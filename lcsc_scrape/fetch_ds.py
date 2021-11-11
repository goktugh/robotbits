"""
Fetch datasheets..

"""
import os
import sys
import hashlib
import time
from scrapeutils import init_logging, init_db, init_session, OUT_DIR

def do_fetch_datasheet(session, conn, id, datasheet_url):
    resp = session.get(datasheet_url)
    if resp.status_code != 200:
        print("Failed fetching: {} datasheet: {}"
            .format(status_code, datasheet_url))
    datasheet_bin = resp.content
    datasheet_len = len(datasheet_bin)
    hashobj = hashlib.sha256()
    hashobj.update(datasheet_bin)
    datasheet_sha2 = hashobj.hexdigest()
    content_type = resp.headers.get('Content-type')
    now = int(time.time())
    with conn:
        
        conn.execute("INSERT OR IGNORE INTO datasheets.content "
            "(content_length, content_sha256, data, date_created)"
            " VALUES (?,?,?,?)",
            (datasheet_len, datasheet_sha2, datasheet_bin, now)
            )
        conn.execute("INSERT OR IGNORE INTO datasheets.datasheet "
            "(url,date_created,content_type,content_length, content_sha256)"
            " VALUES (?,?,?,?,?)",
            (datasheet_url, now, content_type, datasheet_len, datasheet_sha2)
            )
        
        print("Saved datasheet from {}".format(datasheet_url))
    ds_dir = os.path.join(OUT_DIR, 'ds')
    with open(os.path.join(ds_dir, '{}.pdf'.format(datasheet_sha2)), 'wb') as f:
        f.write(datasheet_bin)

def main():

    init_logging()
    ds_dir = os.path.join(OUT_DIR, 'ds')
    os.makedirs(ds_dir, exist_ok=True)
    conn = init_db()
    
    session = init_session()
    # Find products which have a datasheet url but we don't have it yet.
    sql ="""
    select id, lcsc_id, datasheet_url
    FROM product
    LEFT OUTER JOIN datasheets.datasheet AS ds ON
    ds.url = product.datasheet_url
    WHERE datasheet_url IS NOT NULL and LENGTH(datasheet_url) >1
    AND ds.url IS NULL
    ORDER BY lcsc_id
"""
    cur = conn.cursor()
    cur.execute(sql)
    try:
        fetch_count = int(sys.argv[1])
    except IndexError:
        fetch_count = 5
    for row in cur:
        id, lcsc_id, datasheet_url = row
        do_fetch_datasheet(session, conn, id, datasheet_url)
        fetch_count -=1
        if fetch_count == 0:
            print("Enough")
            break
    
    
if __name__ == '__main__':
    main()
