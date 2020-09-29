/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cctype>
#include <string.h>
#include <algorithm>
#include <libgen.h>

#include "CLucene/StdHeader.h"
#include "CLucene/_clucene-config.h"

#include "CLucene.h"
#include "CLucene/util/CLStreams.h"
#include "CLucene/util/dirent.h"
#include "CLucene/config/repl_tchar.h"
#include "CLucene/util/Misc.h"
#include "CLucene/util/StringBuffer.h"

// #include "test.h"
#include "mytest.h"
// #include "myhtml/myhtml.h"
#include "myhtml/api.h"

using namespace std;
using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::store;
using namespace lucene::document;

#define DIE(msg, ...) do { fprintf(stderr, msg, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)

struct res_html {
    char  *html;
    size_t size;
};

struct res_html load_html_file(const char* filename)
{
    FILE *fh = fopen(filename, "rb");
    if(fh == NULL) {
        DIE("Can't open html file: %s\n", filename);
    }
    
    if(fseek(fh, 0L, SEEK_END) != 0) {
        DIE("Can't set position (fseek) in file: %s\n", filename);
    }
    
    long size = ftell(fh);
    
    if(fseek(fh, 0L, SEEK_SET) != 0) {
        DIE("Can't set position (fseek) in file: %s\n", filename);
    }
    
    if(size <= 0) {
        fclose(fh);
        
        struct res_html res = {NULL, 0};
        return res;
    }
    
    char *html = (char*)malloc(size + 1);
    if(html == NULL) {
        DIE("Can't allocate mem for html file: %s\n", filename);
    }
    
    size_t nread = fread(html, 1, size, fh);
    if (nread != size) {
        DIE("could not read %ld bytes (%ld bytes done)\n", size, nread);
    }

    fclose(fh);
    
    struct res_html res = {html, (size_t)size};
    return res;
}
/*
void index_html_content() {
    // build html type
    myhtml_t* html = myhtml_create();
    mystatus_t html_res;

    html_res = myhtml_init(html, MyHTML_OPTIONS_DEFAULT, 0, 0);
    if (MYHTML_FAILED(html_res)) {
        perror("can not init myhtml_t html");
        myhtml_destroy(html);
        return;
    }

    myhtml_tree_t* tree = myhtml_tree_create();
    html_res = myhtml_tree_init(tree, html);
    if (MYHTML_FAILED(html_res)) {
        perror("can not init myhtml_t html");
        myhtml_destroy(html);
        myhtml_tree_destroy(tree);
        return;
    }

    struct res_html data = load_html_file("/Users/huangwei/code/prj/serve/nginx_root/luc/mem_alloc.htm");
    // char* html_content = "<html><head><title>11</title></head><body><p>hello world<span> is first</span></p></body></html>";
    myhtml_parse(tree, MyENCODING_UTF_8, data.html, data.size);

    myhtml_tree_node_t* body = myhtml_tree_get_node_html(tree);
    // const char* tagname = myhtml_node_text(body, NULL);
    myhtml_tag_id_t html_id = myhtml_node_tag_id(body);
    size_t len = 4;
    const char* tagname = myhtml_tag_name_by_id(tree, html_id, &len);
    cout << tagname << endl;

    char* target_tag = "p";
    myhtml_collection_t* list = myhtml_get_nodes_by_name(tree, NULL, target_tag, strlen(target_tag), &html_res);
    cout << "h1 list length " << list->length << " : " << list->size << endl;
    
    // mycore_string_t* text = myhtml_node_string(body);

    // cout << html_res << " of status and before parsing html content " << html_content << endl;

    myhtml_tree_destroy(tree);
    myhtml_destroy(html);
}*/

void loop_node(Document* doc, myhtml_tree_t* tree, myhtml_tree_node_t* node, StringBuffer* buf) {
    size_t len;
    myhtml_tag_id_t tag_id = myhtml_node_tag_id(node);
    switch (tag_id)
    {
    case MyHTML_TAG__COMMENT:
    case MyHTML_TAG__UNDEF:
    case MyHTML_TAG__END_OF_FILE:
        cout << "dummy node" << endl;
        break;

    case MyHTML_TAG__TEXT: {
        const char* text = myhtml_node_text(node, &len);
        
        cout << "==============found text line " << len << ":" << strlen(text) << endl;
        cout << text << endl;
        cout << "==============\\/end found text line======" << endl;

        TCHAR text_buf[len];
        STRCPY_AtoT(text_buf, text, len);
        buf->append(text_buf);
        break;
    }
        
    default: {
        myhtml_tree_node_t* child = myhtml_node_child(node);
        while (child != NULL) {
            loop_node(doc, tree, child, buf);
            child = myhtml_node_next(child);
        }
        break;
    } 
    
    }
}

void FileDocument(const char* f, Document* doc, myhtml_tree_t* tree){

    my_test_whisper();

    // Add the path of the file as a field named "path".  Use an indexed and stored field, so
    // that the index stores the path, and so that the path is searchable.
    TCHAR tf[CL_MAX_DIR];
    STRCPY_AtoT(tf,f,CL_MAX_DIR);
    doc->add( *_CLNEW Field(_T("path"), tf, Field::STORE_YES | Field::INDEX_UNTOKENIZED ) );

    // Add the last modified date of the file a field named "modified". Again, we make it
    // searchable, but no attempt is made to tokenize the field into words.
    //doc->add( *_CLNEW Field(_T("modified"), DateTools::timeToString(f->lastModified()), Field::STORE_YES | Field::INDEX_NO));

    // Add the contents of the file a field named "contents".  This time we use a tokenized
	// field so that the text can be searched for words in it.

    // Here we read the data without any encoding. If you want to use special encoding
    // see the contrib/jstreams - they contain various types of stream readers

    /**/

    const char* ext = basename(const_cast<char*>(f));
    if (strstr(ext, ".htm") != NULL) {
        std::cout << "parse html file" << std::endl;
        struct res_html res;
        res = load_html_file(f);

        if (res.size == 0) {
            return;
        }

        myhtml_parse(tree, MyENCODING_UTF_8, res.html, res.size);
        /*
        myhtml_tree_node_t* html = myhtml_tree_get_node_html(tree);
        myhtml_tag_id_t html_tag_id = myhtml_node_tag_id(html);

        
        myhtml_tree_node_t* body = myhtml_tree_get_node_body(tree);
        size_t len;
        const char* text = myhtml_node_text(body, &len);

        std::cout << "parse html file" << std::endl;
        std::cout << html_tag_id << " is textable " << (html_tag_id == MyHTML_TAG__TEXT || html_tag_id == MyHTML_TAG__COMMENT) << "-----" << len << std::endl;
        // std::cout << "node id " << html_tag_id << endl;
        size_t text_size = strlen(text);
        TCHAR wc[text_size];
        STRCPY_AtoT(wc, text, text_size);
        */

        /*char* target_name = "title";
        mystatus_t st;
        myhtml_collection_t* titles = myhtml_get_nodes_by_name(tree, NULL, target_name, strlen(target_name), &st);
        if (titles != NULL && titles->size > 0) {
            myhtml_tree_node_t* node_title = *(titles->list);
            myhtml_tree_node_t* text_of_title = myhtml_node_child(node_title);

            myhtml_tag_id_t tag_id = myhtml_node_tag_id(text_of_title);
            assert(tag_id == MyHTML_TAG_TEXT || tag_id == MyHTML_TAG_COMMENT);

            size_t len;
            const char* text = myhtml_node_text(text_of_title, &len);
            // std::cout << "last empty " << (text[strlen(text) - 1] == '\0') << endl;
            size_t text_size = strlen(text);
            TCHAR wc[text_size];
            STRCPY_AtoT(wc, text, text_size);
            std::cout << "title text is " << text << "------" << _tprintf(wc) << "------" << len << std::endl;

            doc->add( *_CLNEW Field(_T("contents"), wc, Field::STORE_YES | Field::INDEX_TOKENIZED) );
        }*/

        StringBuffer buf;
        loop_node(doc, tree, myhtml_tree_get_node_html(tree), &buf);

        doc->add( *_CLNEW Field(_T("contents"), buf.getBuffer(), Field::STORE_YES | Field::INDEX_TOKENIZED) );
        cout << "[final " << buf.length() << "]:" << endl;
        _tprintf(buf.getBuffer());

        free(res.html);      

    } else {
        std::cout << "parse normal file " << std::endl;

        FILE* fh = fopen(f,"r");
        if ( fh != NULL ){
            StringBuffer str;
            char abuf[1024];
            TCHAR tbuf[1024];
            size_t r;
            do{
                r = fread(abuf,1,1023,fh);
                abuf[r]=0;
                STRCPY_AtoT(tbuf,abuf,r);
                tbuf[r]=0;
                // puts(abuf);
                str.append(tbuf);
            }while(r>0);
            fclose(fh);

            // std::cout << "####index file content " << str.getBuffer() << std::endl;

            doc->add( *_CLNEW Field(_T("contents"), str.getBuffer(), Field::STORE_YES | Field::INDEX_TOKENIZED) );
        }
    }
}

void indexDocs(IndexWriter* writer, const char* directory, myhtml_tree_t* tree) {
    vector<string> files;
    std::sort(files.begin(),files.end());
    Misc::listFiles(directory,files,true);
    vector<string>::iterator itr = files.begin();
    
    // Re-use the document object
    Document doc;
    int i=0;
    while ( itr != files.end() ){
        const char* path = itr->c_str();
        printf( "adding file %d: %s\n", ++i, path );

        doc.clear();
        FileDocument( path, &doc, tree);
        writer->addDocument( &doc );
        ++itr;
    }
}
void IndexFiles(const char* path, const char* target, const bool clearIndex){
	IndexWriter* writer = NULL;
	lucene::analysis::WhitespaceAnalyzer an;
	
	if ( !clearIndex && IndexReader::indexExists(target) ){
		if ( IndexReader::isLocked(target) ){
			printf("Index was locked... unlocking it.\n");
			IndexReader::unlock(target);
		}

		writer = _CLNEW IndexWriter( target, &an, false);
	}else{
		writer = _CLNEW IndexWriter( target ,&an, true);
	}

    //writer->setInfoStream(&std::cout);

    // We can tell the writer to flush at certain occasions
    //writer->setRAMBufferSizeMB(0.5);
    //writer->setMaxBufferedDocs(3);

    // To bypass a possible exception (we have no idea what we will be indexing...)
    writer->setMaxFieldLength(0x7FFFFFFFL); // LUCENE_INT32_MAX_SHOULDBE
    
    // Turn this off to make indexing faster; we'll turn it on later before optimizing
    writer->setUseCompoundFile(false);

	uint64_t str = Misc::currentTimeMillis();

    /**
     * start to constructing html file doc index  
     * components
     */
    cout << "before to index html doc" << endl;
    myhtml_t* html = myhtml_create();
    mystatus_t html_res;

    html_res = myhtml_init(html, MyHTML_OPTIONS_DEFAULT, 0, 0);
    if (MYHTML_FAILED(html_res)) {
        perror("can not init myhtml_t html");
        myhtml_destroy(html);
        return;
    }

    myhtml_tree_t* tree = myhtml_tree_create();
    html_res = myhtml_tree_init(tree, html);
    if (MYHTML_FAILED(html_res)) {
        perror("can not init myhtml_t html");
        myhtml_destroy(html);
        myhtml_tree_destroy(tree);
        return;
    }

    // index_html_content();

	indexDocs(writer, path, tree);

    myhtml_tree_destroy(tree);
    myhtml_destroy(html);
	
    // Make the index use as little files as possible, and optimize it
    writer->setUseCompoundFile(true);
    writer->optimize();
	
    // Close and clean up
    writer->close();
	_CLLDELETE(writer);

	printf("Indexing took: %d ms.\n\n", (int32_t)(Misc::currentTimeMillis() - str));
}
