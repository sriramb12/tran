#!/run/pkg/TWW-python-/2.7.2/bin/python
# -*- coding: utf-8 -*-

import pprint
import smtplib
import datetime
import unicodedata
import imaplib, email
import MySQLdb as mdb
from time import time
import re, sys, os, xlwt
from email.mime.text import MIMEText
import email.encoders, email.mime.base
from email.mime.multipart import MIMEMultipart


def get_today(): return datetime.datetime.fromtimestamp( time() ).strftime('%d/%b/%Y')

def get_10days(): return ( datetime.date.today() - datetime.timedelta( days = 10 ) ).strftime('%d/%b/%Y')

def get_weekly_label():
    week = str( datetime.date.today().isocalendar()[1] ).zfill(2)
    year = currentYear = datetime.datetime.now().year
    return "Weekly Report {0}WW{1}".format(year, week)

def get_full_weekly_label():
    return get_weekly_label() + " &mdash; Upload date from <b>{0}</b> until <b>{1}</b>".format( get_10days(), get_today() )

def html_begin():
    return """<html>
        <head>
        <style>
            body {
                font-family: monospace;
                font-size: 9pt;
            }
            table, td {
                border-collapse: collapse;
                border: 1px solid black;
            }
            th {
                border: 1px solid black;
                border-collapse: collapse;
                font-weight: bold;
                text-align: center;
                background-color: WhiteSmoke;
            }
            td#transferstatus, td#createdin, td#created_in, td#expiresat, td#expires_in, td#days_remaining, td#transfer_status, td#downloaded_in, td#to_email {
                text-align: center;
            }
        </style>
        </head>
        <body>"""


def html_end(): return "</body></html>"


def load_data():
    conn = mdb.connect('dtms01-tx30', 'ra5063', 'freescale', 'data_transfer')
    cur = conn.cursor(mdb.cursors.DictCursor)

    SQL = '''
        SELECT DISTINCT
            t2.secure_keyword,
            t2.file_keyword,
            t1.file_name,
            t2.description,
            t2.to_email,
            DATE_FORMAT(t2.upload_date, '%d/%b/%Y') AS 'created_in',
            DATE_FORMAT(t2.download_date, '%d/%b/%Y') AS 'downloaded_in',
            DATE_FORMAT(t1.expirationdate, '%d/%b/%Y') AS 'expires_in',
            TIMESTAMPDIFF(DAY, NOW(), t1.expirationdate) AS 'days_remaining',
            IF(t2.download_date, 'COMPLETE', 'PENDING') AS 'transfer_status'
        FROM
            dt_transactions AS t1,
            dt_keyword_mapping AS t2
        WHERE
            t1.file_keyword = t2.file_keyword
            AND t2.from_email IN ("TTZ746", "rzpadm")
            AND t2.upload_date BETWEEN date_sub(NOW(), INTERVAL 10 DAY) AND NOW()
            ORDER BY UNIX_TIMESTAMP(t1.timestamp) ASC
          '''

    cur.execute(SQL)
    dataset = cur.fetchall()
    if len(dataset) <= 0:
        return None

    final = []
    for rec in dataset:
        #print "DEBUG: dataset = ", pprint.pprint(rec, indent=10, width=100, depth=20), " ++++++++++++++++++++++++++++++ "
        if rec['days_remaining'] <= 0:
            rec['transfer_status'] = 'EXPIRED'
        final.append(rec)

    return final


def get_metainfo(keyword, metainfo):
    for mi in metainfo:
        if keyword == mi['Keyword']:
            return mi['Description'], mi['Customer/Subscriber']


def metaid_customize(metaid): return metaid.replace('/','').replace(' ','').lower()


def html_table_meta(headers):
    #print "DEBUG: headers contain", pprint.pprint(headers, indent=10, width=100, depth=20)
    translator = {
	'description': 'File Description', 
	'to_email': 'Recipient', 
	'file_name': 'File Name', 
	'secure_keyword': 'Secure Keyword', 
	'expires_in': 'Expires in', 
	'transfer_status': 'Transfer Status', 
	'days_remaining': 'Days Remaining', 
	'downloaded_in': 'Downloaded in', 
	'created_in': 'Created in', 
	'file_keyword': 'File Keyword' }

    ignore = ('file_name', 'secure_keyword', 'file_keyword', 'description')

    table = "<thead><tr><th id='metainfo'>General Information</th>"
    for head in headers:
        if head not in ignore:
            mID = metaid_customize( translator[head] )
            table += "<th id='{0}'>{1}</th>".format(mID, translator[head] )
    return table + '</tr></thead>'


def html_table_transfer(record, headers):
    just_valid = ('to_email', 'created_in', 'expires_in', 'days_remaining', 'transfer_status', 'downloaded_in')

    content = """<td id='metainfo'>
                    Secure Keyword: {0}<br>
                    File Keyword: {1}<br>
                    File Name: {2}<br>
                    Description: {3}<br>
              </td>""".format(record['secure_keyword'], record['file_keyword'], record['file_name'], record['description'])

    for h in just_valid:
        mID = metaid_customize( str(h) )
        #print 'before call metaid_customize({0})'.format(h)
        if h == 'transfer_status':
            if record[h] == 'PENDING':
                content += "<td id='" + str(mID) + "'><span style='background-color:#FFFF00;color:#000;'>&nbsp;" + str(record[h]) + "&nbsp;</span></td>"
            elif record[h] == 'COMPLETE':
                content += "<td id='" + str(mID) + "'><span style='background-color:#00FF00;color:#FFF;'>&nbsp;" + str(record[h]) + "&nbsp;</span></td>"
            elif record[h] == 'EXPIRED':
                content += "<td id='" + str(mID) + "'><span style='background-color:#FF0000;color:#FFF;'>&nbsp;" + str(record[h]) + "&nbsp;</span></td>"
        else:
            content += "<td id='" + str(mID) + "'>" + str(record[h]) + "</td>"
    return '<tr>' + content + '</tr>'


def xls_init():
    font_header = xlwt.Font()
    font_header.name = 'Courier New'
    font_header.bold = True
    font_header.height = 12*0x14

    font_body = xlwt.Font()
    font_body.name = 'Courier New'
    font_body.height = 10*0x14

    borders = xlwt.Borders()
    borders.left   = 2
    borders.right  = 2
    borders.top    = 2
    borders.bottom = 2

    al = xlwt.Alignment()
    al.horz = xlwt.Alignment.HORZ_CENTER
    al.vert = xlwt.Alignment.VERT_CENTER

    normal_style = xlwt.XFStyle()
    normal_style.font = font_body

    header_style = xlwt.XFStyle()
    header_style.borders = borders
    header_style.alignment = al
    header_style.font = font_header

    wb = xlwt.Workbook()
    return wb, header_style, normal_style

def compute_stats(record, stats):
    if record:
        stats['total_keywords'] += 1

    if record['transfer_status'] == 'PENDING':
        stats['transfers_pending'] += 1
        stats['about_to_expire'] += 1

    elif record['transfer_status'] == 'EXPIRED':
            stats['expiring_today'] += 1

    elif record['transfer_status'] == 'COMPLETE':
        stats['transfers_complete'] += 1

    return stats


### MAIN ###
if __name__ == "__main__":
    toList = [ "transbug@nxp.com", "DMSTWEB@nxp.com" ]
    FAMEDC = "FAMEDC@nxp.com"
    print "DEBUG: will notify the following email list", toList, FAMEDC

    ## loading email data
    records = load_data()
    #print "DEBUG: records contains \n", pprint.pprint(records, indent=10, width=100, depth=20)

    stats = {
            'transfers_pending': 0,
            'transfers_complete': 0,
            'expiring_today': 0,
            'about_to_expire': 0,
            'total_keywords': 0
            }

    subject = "Intercom {0}".format(get_weekly_label())
    sheet_name = 'intercom_activities'
    print "DEBUG: e-mail subject will be", subject

    ## init excel output
    xls, header_style, normal_style = xls_init()
    ws = xls.add_sheet(sheet_name)

    headers = ('secure_keyword', 'file_keyword', 'file_name', 'description', 'to_email', 'created_in', 'expires_in', 'days_remaining', 'transfer_status', 'downloaded_in')
    final_header = None
    table_body = ''

    if records:
        ## write down the today's subject/timestamp into the XLS
        ws.write_merge(0, 0, 0, 8, subject, header_style)
        sheet_line_count = 1

        for rec in records:
            print "DEBUG: processing", pprint.pprint(rec, indent=1, width=100, depth=2)

            ## we process the header once
            if not final_header:
                final_header = html_table_meta(headers)
                #print "DEBUG: final_header will be", final_header
                for j, h in enumerate(headers):
                    ws.write(sheet_line_count, j, h, header_style)
                sheet_line_count += 1 ## jumps out from the first sheet line

            ## building table body
            table_body += html_table_transfer(rec, headers)
            count = 0

            ## computing stats
            stats = compute_stats(rec, stats)

            for h in headers:
                ws.write(sheet_line_count, count, rec[h], normal_style)
                count += 1
            sheet_line_count += 1
    else:
        print "No new email... bye!"; sys.exit(0) ## EXIT MAIN

    stats_html = """Items not downloaded yet: <b>{transfers_pending}</b><br>
                    Items expired/not downloaded: <b>{expiring_today}</b><br>
                    Total of keywords: <b>{total_keywords}</b>
                    (<b>{transfers_complete}</b> are complete)</p>""".format(**stats)
    final_html = "<p>" +  get_full_weekly_label() + '<br>' + stats_html + html_begin() + "<table cellspacing=10 cellpadding=10 width='100%'>" + final_header + "<tbody>" + table_body + "</tbody>" + html_end()

    ## saving worksheet
    xls.save(sheet_name + '.xls')

    ## Multipart email (header + html body + xls attachment)
    msg = MIMEMultipart('alternative')
    msg['To'] = ", ".join(toList)
    msg['From'] = FAMEDC
    msg['Subject'] = subject

    msg.attach( MIMEText(" ", 'plain') )
    msg.attach( MIMEText(final_html, 'html') )

    ## now attach the .xls file
    fileMsg = email.mime.base.MIMEBase('application','vnd.ms-excel')
    fileMsg.set_payload( file(sheet_name + '.xls').read() )
    ## ...dont forget to encode
    email.encoders.encode_base64(fileMsg)
    fileMsg.add_header('Content-Disposition','attachment;filename=' + sheet_name + '.xls')
    msg.attach(fileMsg)

    print "DEBUG: Send the email via our own SMTP server"
    s = smtplib.SMTP("remotesmtp.freescale.net")
    try:
        s.sendmail(FAMEDC, toList, msg.as_string())
    finally:
        s.quit()
    print "DEBUG: bye!"; sys.exit(0)
