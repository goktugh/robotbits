import requests
import logging
import sqlite3
import os
import http.client as http_client

OUT_DIR='output'

def init_logging():
    logging.basicConfig(level=logging.DEBUG)

    #verbose debug
    http_client.HTTPConnection.debuglevel = 1

def init_session():
    
    session = requests.Session()
    session.headers['User-Agent'] = 'Mark LCSC scraper'
    return session
    

INIT_SQLS = (
"""
    CREATE TABLE IF NOT EXISTS category (id integer primary key, name, date_created NOT NULL)
""",
"""
    CREATE TABLE IF NOT EXISTS product (
        id integer NOT NULL primary key, 
        lcsc_id NOT NULL, -- e.g. C12345
        manufacturer,
        mpn,
        package,
        product_url,
        datasheet_url,
        name, date_created NOT NULL)
""",
# Store the length in bytes and the sha256 hash (as ascii hex)
# So we can identify duplicates easily.
"""
    CREATE TABLE IF NOT EXISTS datasheets.datasheet (
        url primary key, 
        date_created NOT NULL, 
        content_type, 
        content_length INTEGER,
        content_sha256)
""",

# Store unique content in this table:
"""
    CREATE TABLE IF NOT EXISTS datasheets.content (
        content_length INTEGER NOT NULL,
        content_sha256 NOT NULL,
        data BLOB,
        date_created NOT NULL,
        PRIMARY KEY (content_length, content_sha256)
        )
""",

)

def init_db():
    print("init_db...")
    main_db = os.path.join(OUT_DIR, 'main.sqlite3')
    datasheets_db = os.path.join(OUT_DIR, 'datasheets.sqlite3')
    ds_conn = sqlite3.connect(datasheets_db)
    ds_conn.execute("pragma journal_mode=WAL")
    ds_conn.close()
    
    conn = sqlite3.connect(main_db)
    conn.execute("pragma journal_mode=WAL")
    conn.execute("attach ? as datasheets",
        (datasheets_db,) )
    for sql in INIT_SQLS:
        conn.execute(sql)
    conn.commit()
    return conn

