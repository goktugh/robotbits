"""
Scraper for LCSC's catalogue.

They don't provide an obvious catalogue for download - only for the
smt assembly parts.

We need to open a web session, and fetch a html first, to get the cookies
and x-csrf-token - as the api is intended for use by Javascript code.
After that we can submit api requests.

"""
import requests
import logging
import html.parser
import os
import json
import time
import sqlite3

from scrapeutils import init_logging, init_db, init_session, OUT_DIR

#verbose debug
# http_client.HTTPConnection.debuglevel = 1

PRODUCTS_URL = 'https://lcsc.com/products'

def parse_category_ids(htmltext):
    """
        Example:
        <div data-id="601" class="catalogue-list-item">Analog ICs <br></div>

    """
    class CategoriesParser(html.parser.HTMLParser):
        categories = []
        last_category_id = None
        
        def handle_starttag(self, tag, attrs):
            category_id = None
            for name, value in attrs:
                if name == 'data-id':
                    category_id = value
            if category_id:
                self.last_category_id = category_id
                self.text = ''

        def handle_data(self, data):
            if self.last_category_id:
                self.text += data

        def handle_endtag(self, tag):
            if self.last_category_id:
                id_and_name = (self.last_category_id, self.text.strip())
                self.last_category_id = None
                self.text = ''
                self.categories.append(id_and_name)
                print(id_and_name)
                
    parser = CategoriesParser()
    parser.feed(htmltext)
    return parser.categories

def download_category_page(session, category_id, page):
    url = 'https://lcsc.com/api/products/search'
    params = {
        'current_page': str(page),
        'category': str(category_id),
        'in_stock': "true",
        'is_RoHS': 'false',
        'show_icon': 'false',
        'search_content': '',
        'limit': '1000',
            }
            
    out_filename = os.path.join(OUT_DIR, 'cat_{}_page_{}.json'.format(category_id, page))
        
    if not os.path.exists(out_filename):        
        resp = session.post(url,
            data=params)
        if resp.status_code != 200:
            print(resp.text)
        resp.raise_for_status()
        
        json_out = resp.json()
        if json_out.get("success") in ("true", True):
            with open(out_filename, 'wt') as f:
                json.dump(json_out, f, indent=2)
            print("Saved "+ out_filename)
        else:
            print("Failed " + out_filename)
    return out_filename

def load_db_products(conn, filename):
    now = int(time.time())
    with conn:
        with open(filename, 'rt') as f:
            jsondoc = json.load(f)
        products_data = jsondoc.get('result',{}).get('data')
        if products_data:
            print("load_db_products: from {} loading {} products"
                .format(filename, len(products_data)))
            for row in products_data:
                conn.execute("""
                    INSERT OR IGNORE INTO product
                    (id, lcsc_id, product_url,
                        manufacturer, mpn, datasheet_url, date_created)
                    values (?,?,?,
                        ?,?,?,?)""",
                    (row['id'], row['number'], row['url'],
                    row['manufacturer'].get('en'),
                    row['info']['number'],
                    row['datasheet'].get('pdf'),
                    now # date_created
                    )
                    )                

def load_db_categories(conn, categories_with_ids):
    now = int(time.time())
    with conn:
        for id,name in categories_with_ids:
                conn.execute("""
                    INSERT OR IGNORE INTO category
                    (id, name,date_created)
                    values (?,?,?)""",
                    (id, name, now) )

def download_category_all(session, category_id, conn):
    p1_filename = download_category_page(session, category_id, 1)
    # Get the number of pages.
    p1_json = json.load(open(p1_filename))
    num_pages = int(p1_json['result']['total_page'])
    del p1_json
    # Download ..
    for page in range(1,num_pages+1):
        filename = download_category_page(session, category_id, page)
        if os.path.exists(filename):
            load_db_products(conn, filename)

def main():

    init_logging()
    os.makedirs(OUT_DIR, exist_ok=True)
    conn = init_db()
    
    session = init_session()
    
    # Get a HTML page, so we can set up cookies and csrf token.
    url0 = PRODUCTS_URL
    
    resp = session.get(url0)
    resp.raise_for_status()
    with open('products.html', 'wt') as f:
        print(resp.text, file=f)
    categories_with_ids = parse_category_ids(resp.text)
    load_db_categories(conn, categories_with_ids)
    
    # Parse out <script type="text/javascript">
    # $.ajaxSetup({ headers: { 'X-CSRF-TOKEN': 'KLC1AqpuXPObMgMsVXrPnwQojHprxCExsXs5Vh0C'}});
    # </script>
    
    page_text = resp.text
    lines = page_text.split('\n')
    for line in lines:
        if 'X-CSRF-TOKEN' in line:
            #print(line)
            x_csrf_token = line.split("'")[-2]
    
    print("csrf token is {}".format(x_csrf_token))
    session.headers['X-CSRF-TOKEN'] = x_csrf_token
    
    # Now search for products....
    for category_id, category_name in categories_with_ids:
        if 'ICs' in category_name:
            download_category_all(session, category_id, conn)

if __name__ == '__main__':
    main()
