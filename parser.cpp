#include "parser.h"

Parser::Parser()
{

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QDebug>
#include <QByteArray>

#include "3dparty/md4c/src/md4c-html.h"



/* Global options. */
static unsigned parser_flags = 0;
#ifndef MD4C_USE_ASCII
static unsigned renderer_flags = MD_HTML_FLAG_DEBUG | MD_HTML_FLAG_SKIP_UTF8_BOM;
#else
static unsigned renderer_flags = MD_HTML_FLAG_DEBUG;
#endif
static int want_fullhtml = 1;


/*********************************
 ***  Simple grow-able buffer  ***
 *********************************/

/* We render to a memory buffer instead of directly outputting the rendered
 * documents, as this allows using this utility for evaluating performance
 * of MD4C (--stat option). This allows us to measure just time of the parser,
 * without the I/O.
 */

struct membuffer {
    char* data;
    size_t asize;
    size_t size;
};

static void
membuf_init(struct membuffer* buf, MD_SIZE new_asize)
{
    buf->size = 0;
    buf->asize = new_asize;
    buf->data = (char*)malloc(buf->asize);
}

static void
membuf_fini(struct membuffer* buf)
{
    if(buf->data)
        buf->data = nullptr;
        // free(buf->data);
}

static void
membuf_grow(struct membuffer* buf, size_t new_asize)
{
    buf->data = (char*)realloc(buf->data, new_asize);
    buf->asize = new_asize;
}

static void
membuf_append(struct membuffer* buf, const char* data, MD_SIZE size)
{
    if(buf->asize < buf->size + size)
        membuf_grow(buf, buf->size + buf->size / 2 + size);
    memcpy(buf->data + buf->size, data, size);
    buf->size += size;
}


/**********************
 ***  Main program  ***
 **********************/

static void
process_output(const MD_CHAR* text, MD_SIZE size, void* userdata)
{
    membuf_append((struct membuffer*) userdata, text, size);
}

QString Parser::Parse(QString markdown, Mode mode)
{
    if (mode == GitHub)
        parser_flags |= MD_DIALECT_GITHUB;
    else
        parser_flags |= MD_DIALECT_COMMONMARK;

    parser_flags |= MD_FLAG_TABLES | MD_FLAG_TASKLISTS;
    // size_t n;
    struct membuffer buf_in = {nullptr, 0, 0};
    struct membuffer buf_out = {nullptr, 0, 0};

    QString out;


    membuf_init(&buf_in, 32 * 1024);

    QByteArray array = markdown.toLocal8Bit();
    buf_in.data = array.data();
    buf_in.size = strlen(buf_in.data);

    // FILE* in = fmemopen(markdown.toLocal8Bit().data(), markdown.length(), "r");

    /* Read the input file into a buffer.
    while(1) {
        if(buf_in.size >= buf_in.asize)
            membuf_grow(&buf_in, buf_in.asize + buf_in.asize / 2);

        n = fread(buf_in.data + buf_in.size, 1, buf_in.asize - buf_in.size, in);
        if(n == 0)
            break;
        buf_in.size += n;
    }
    */

    /* Input size is good estimation of output size. Add some more reserve to
     * deal with the HTML header/footer and tags. */
    membuf_init(&buf_out, (MD_SIZE)(buf_in.size + buf_in.size/8 + 64));

    /* Parse the document. This shall call our callbacks provided via the
     * md_renderer_t structure. */

    md_html(buf_in.data, (MD_SIZE)buf_in.size, process_output, (void*) &buf_out,
                  parser_flags, renderer_flags);


    /* Write down the document in the HTML format. */
    if(want_fullhtml) {
        out.append("<!DOCTYPE html>\n");
        out.append("<html>\n");
        out.append("<head>\n");
        out.append("<title></title>\n");
        out.append("<meta name=\"generator\" content=\"md2html\">\n");
        out.append("</head>\n");
        out.append("<body>\n");
    }

    out.append(buf_out.data);

    if(want_fullhtml) {
        out.append("</body>\n");
        out.append("</html>\n");
    }

    // qDebug() << buf_in.data;


    membuf_fini(&buf_in);
    membuf_fini(&buf_out);

    return out;
}
