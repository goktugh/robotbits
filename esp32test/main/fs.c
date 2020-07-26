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
    // Example: Create a log file with higher id than any existing.
    int highest_id = find_highest_numeric_file(logs_path);
    highest_id += 1;
    char log_fname[256];
    snprintf(log_fname, sizeof(log_fname), "%s/%04d.log", logs_path, highest_id);
    ESP_LOGI(TAG, "Opening log %s", log_fname);
    f = fopen(log_fname, "w");
    if (f != NULL) {
        fprintf(f, "Hi there\n");
        fclose(f);
    }
}

// SUPER SIMPLE WEB SERVER:
// only get, gets files from the fs
static esp_err_t download_get_handler(httpd_req_t *req)
{
    // TODO
    return ESP_FAIL;
}

static int server_data;

void web_server_init()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
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
