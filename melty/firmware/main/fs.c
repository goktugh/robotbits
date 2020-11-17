#include "fs.h"

#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "esp_http_server.h"

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

/*
 * Mostly taken from example: wear_levelling
 */

static const char *TAG = "my_fs";

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

// Mount path for the partition
const char *base_path = FLASH_PATH;

// scan directory path, find the highest id file 
// for example 1.log or 0420.txt and return the number.
// Ignore non-numeric parts of the path.
static int find_highest_numeric_file(const char *path)
{
    DIR *d = opendir(path);
    if (d == NULL) return 0;
    struct dirent *entry;
    int highest=0;
    while(1) {
        entry = readdir(d);
        if (entry == NULL) {
            break;
        } else {
            int num=atoi(entry->d_name);
            if (num > highest) {
                highest = num;
            }
        }
    }
    return highest;
}


void fs_init()
{
    ESP_LOGI(TAG, "Mounting FAT filesystem");
    // To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formated before
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount(base_path, "storage", &mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
    FILE *f = fopen(FLASH_PATH "/hello2.txt", "w");
    if (f != NULL) {
        fprintf(f, "Hello, World\n");
        fclose(f);
        ESP_LOGI(TAG, "FS working ok");
    } else {
        ESP_LOGE(TAG, "FS: Failed to open file");
    }
    // Example make a directory.
    int mode = 0777; // octal mode for new dir (probably noop anyway)
    const char * logs_path = FLASH_PATH "/logs";
    mkdir(logs_path, mode);
    // Example list files:
    {
        DIR *d = opendir(base_path);
        if (d) {
            struct dirent *entry;
            while(1) {
                entry = readdir(d);
                if (entry == NULL) {
                    break;
                } else {
                    ESP_LOGI(TAG, "FS: I see dirent %s", entry->d_name);
                }
            }
        }
    }
}

static char chunk_buf[4096];

// Arbitrary:
#define FILE_PATH_MAX 200

// SUPER SIMPLE WEB SERVER:
// only get, gets files from the fs

static void map_path(httpd_req_t *req, char *path_out)
{
    strncpy(path_out, FLASH_PATH, FILE_PATH_MAX);
    strncat(path_out, req->uri, FILE_PATH_MAX - 2);
    path_out[FILE_PATH_MAX - 1] = '\0';
}

static esp_err_t download_directory_index(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Serving directory index for %s", req->uri);
    char filepath[FILE_PATH_MAX];
    // Copy the req->uri and remove the last character - always /
    map_path(req, filepath);
    filepath[FILE_PATH_MAX - 1] = '\0'; // esure null terminated
    filepath[strlen(filepath) -1 ] = '\0'; // one char shorter.
    struct stat file_stat;

    // Try to stat the dir,
    if (stat(filepath, &file_stat) == -1) {
        // Failed to stat.
        ESP_LOGW(TAG, "Failed to stat %s", filepath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error");
        return ESP_FAIL;
    }

    // Set up headers
    httpd_resp_set_hdr(req, "Connection", "close"); // important- otherwise connection hangs.
    httpd_resp_set_type(req, "text/html; charset=us-ascii");
    
    char buf[200];
    int len = snprintf(buf, sizeof(buf), "<!DOCTYPE html><html><body><pre>dir index %s</pre>\n\n", req->uri);
    if (len < sizeof(buf)) {
        httpd_resp_send_chunk(req, buf, len);
    }

    DIR *d = opendir(filepath);
    if (d) {
        struct dirent *entry;
        while(1) {
            entry = readdir(d);
            if (entry == NULL) {
                break;
            } else {
                // If filename starts with chr(0xff) it is an error.
                if (entry->d_name[0] == 0xff) {
                    // Bug or filesystem corruption.
                    continue;
                }
                // Try to stat the file.
                char filename_in_dir[FILE_PATH_MAX];
                len = snprintf(filename_in_dir, sizeof(filename_in_dir), "%s/%s", filepath, entry->d_name);
                if (len < FILE_PATH_MAX) { // Skip listing file if the buffer overflows
                    struct stat filestat;
                    memset(&filestat, 0, sizeof(filestat)); 
                    stat(filename_in_dir, &filestat);
                    char type_ch = ' ';
                    if (S_ISDIR(filestat.st_mode)) {
                        type_ch = '/';
                    }
                    
                    len = snprintf(buf, sizeof(buf),
                            "<div><a href=\"%s\">%s%c</a> %ld </div>\n", entry->d_name, entry->d_name, type_ch, filestat.st_size);
                    if (len < sizeof(buf)) {
                        httpd_resp_send_chunk(req, buf, len);
                    }
                }
            }
        }
    }
    
    
    
    // Signal end of resp and ok.
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t download_file(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];
    FILE *fd = NULL;
    struct stat file_stat;
    map_path(req, filepath);
    
    // Try to stat the file,
    if (stat(filepath, &file_stat) == -1) {
        // stat fails: assume not found.
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not found");
        return ESP_FAIL;
    }
    // If it is a directory 
    if (file_stat.st_mode & S_IFDIR) {
        // Send a redirect, with the / added.
        char redirpath[FILE_PATH_MAX];
        strncpy(redirpath, req->uri, FILE_PATH_MAX);
        strncat(redirpath, "/", FILE_PATH_MAX - 2);
        httpd_resp_set_hdr(req, "Location", redirpath);
        httpd_resp_set_status(req, "301 Moved");
        const char * body = "Moved";
        httpd_resp_send(req, body, strlen(body));
        ESP_LOGI(TAG, "Sending redirect to %s", redirpath);
        return ESP_OK;
    }
    // Otherwise serve as file
    // Convert size to a string and set as a header (buffer must remain valid until sent)
    char size_str[20];
    snprintf(size_str, sizeof(size_str), "%ld", file_stat.st_size);
    httpd_resp_set_hdr(req, "Content-length", size_str);
    fd = fopen(filepath, "r");
    if (! fd) {
        // Should not happen.
        ESP_LOGW(TAG, "unable to open file after stat ok");
        return ESP_FAIL;
    }
    
    httpd_resp_set_type(req, "text/plain");
    while (1) {
        // Read chunk from file
        int chunksize = fread(chunk_buf,1, sizeof(chunk_buf), fd);
        if (chunksize == 0) break;
        if (httpd_resp_send_chunk(req, chunk_buf, chunksize) != ESP_OK) {
            // Failed, stop.
            break;
        }
    }
    fclose(fd);
    // Signal end of resp and ok.
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t download_get_handler(httpd_req_t *req)
{
    // If path ends with a / then it is a directory and we should make
    // and index.
    const char *uri=req->uri;
    int urilen = strlen(uri);
    // If it's too long, stop now.
    const int urilen_max = FILE_PATH_MAX - strlen(FLASH_PATH) - 2;
    if (urilen > urilen_max) {
        // Too long.
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Filename too long");
        return ESP_FAIL;
    }
    if (uri[urilen - 1] == '/') {
        // is a directory
        return download_directory_index(req);
    }
    return download_file(req);
}

void web_server_init()
{
    static int server_data;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    httpd_handle_t server = NULL;
    ESP_LOGI(TAG, "Starting HTTP Server");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start file server!");
        return;
    }
    /* URI handler for getting uploaded files */
    httpd_uri_t file_download = {
        .uri       = "/*",  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = download_get_handler,
        .user_ctx  = &server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_download);
    ESP_LOGI(TAG, "http server ok");
}
