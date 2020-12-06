/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include <iostream>
#include <stdio.h>
#include <bitset>

#include "CLucene/StdHeader.h"
#include "CLucene/_clucene-config.h"

#include "CLucene.h"
#include "CLucene/config/repl_tchar.h"
#include "CLucene/config/repl_wchar.h"
#include "CLucene/util/Misc.h"
#include "CLucene/queryParser/MultiFieldQueryParser.h"

#include "CLucene/analysis/LanguageBasedAnalyzer.h"
#include "tool.h"

#include <string>
#include <cstdlib>

using namespace std;
using namespace lucene::analysis;
using namespace lucene::index;
using namespace lucene::util;
using namespace lucene::queryParser;
using namespace lucene::document;
using namespace lucene::search;


/*void convert_multi_byte_to_wchar1(const char* src, TCHAR* dest) {
    size_t src_len = strlen(src);
    if (src_len == 0) {
        return;
    }

    if (dest == nullptr) {
        return;
    }

    const char* src_end = src + src_len;
    TCHAR chr;
    int chr_cnt;
    size_t c = 0;
    while (true) {
        chr_cnt = mbtowc(&chr, src, src_end - src);
        if (chr_cnt == -1) {
            break;
        }
        dest[c++] = chr;
        src += chr_cnt;
    }
}*/


void SearchFiles(const char* index){
    // standard::StandardAnalyzer analyzer;
    LanguageBasedAnalyzer analyzer;
    analyzer.setLanguage(_T("cjk"));

    // showing ucs4 wchar
    cout << "showing ucs4 char in wchar" << endl;
    TCHAR myname[3] = {
        0x9ec4,
        0x4f1f,
        0x0,
    };
    _tprintf(_T("my name1: %s\n"), myname);

    char* hard_coded_seq = "中午";
    size_t hard_chr_cnt = strlen(hard_coded_seq);
    // char* hard_coded_seq_end = hard_coded_seq + hard_chr_cnt;
    TCHAR wchr_seq[hard_chr_cnt];
    convert_multi_byte_to_wchar(hard_coded_seq, wchr_seq);
    _tprintf(_T("seq: %s\n"), wchr_seq);
    
    // TCHAR chr;
    // while (true) {
    //     int wchr_cnt = mbtowc(&chr, hard_coded_seq, hard_coded_seq_end - hard_coded_seq);
    //     if (wchr_cnt == -1) {
    //         break;
    //     }
    //     _tprintf(&chr);
    //     hard_coded_seq += wchr_cnt;
    // }

    // char* myname_1 = "黄伟";
    // cout << "myname_1 len " << strlen(myname_1) << endl;

    const size_t tchar_size = sizeof(TCHAR) * 8;
    bitset<tchar_size> bin_pres(0x9ec4);

    cout << "binary represention of 0x9ec4 is " << bin_pres << endl;

    char line[80];
    TCHAR tline[80];
    TCHAR* buf;

    bool first_search = true;

    IndexReader* reader = IndexReader::open(index);
    while (true) {
        printf("Enter query string: ");
        char* tmp ;
        if (first_search) {
            first_search = false;
            tmp = "文字,";
            strcpy(line, tmp);
        } else {
            tmp = fgets(line,80,stdin);
            line[strlen(line)-1]=0;
        }
        if ( tmp == NULL ) continue;

        IndexReader* newreader = reader->reopen();
        if ( newreader != reader ){
            _CLLDELETE(reader);
            reader = newreader;
        }


        IndexSearcher s(reader);

        if ( strlen(line) == 0 )
            break;
        // STRCPY_AtoT(tline,line,80);
        size_t convert_size = convert_multi_byte_to_wchar(line, tline);
        tline[convert_size] = 0;
        // Query* q = QueryParser::parse(tline,_T("contents"),&analyzer);

        const TCHAR* cols[3] = {
            _T("contents"),
            _T("heading"),
        };

        MultiFieldQueryParser p(cols, &analyzer);
        p.setDefaultOperator(MultiFieldQueryParser::OR_OPERATOR);
        Query* q = p.parse(tline);

        TermSet all_terms;
        q->extractTerms(&all_terms);
        _tprintf(_T("terms size %d\n"), all_terms.size());
        for (Term* s : all_terms) {
            _tprintf(_T("field:%s => value:%s\n"), s->field(), s->text());
        }

        buf = q->toString(_T("contents"));
        _tprintf(_T("Searching for: %s\n\n"), buf);
        _CLDELETE_LCARRAY(buf);

        uint64_t str = Misc::currentTimeMillis();
        Hits* h = s.search(q);
        uint32_t srch = (int32_t)(Misc::currentTimeMillis() - str);
        str = Misc::currentTimeMillis();

        for ( size_t i=0;i<h->length();i++ ){
            Document* doc = &h->doc(i);
            //const TCHAR* buf = doc.get(_T("contents"));
            _tprintf(_T("%d. %s - %f\n"), i, doc->get(_T("path")), h->score(i));
        }

        printf("\n\nSearch took: %d ms.\n", srch);
        printf("Screen dump took: %d ms.\n\n", (int32_t)(Misc::currentTimeMillis() - str));

        _CLLDELETE(h);
        _CLLDELETE(q);

        s.close();
    }
    reader->close();
    _CLLDELETE(reader);
}