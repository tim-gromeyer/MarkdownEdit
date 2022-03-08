#include "parser.h"

Parser::Parser()
{

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QTextStream>
#include <QDataStream>

#include "md4c-html.h"



/* Global options. */
static unsigned parser_flags = 0;
#ifndef MD4C_USE_ASCII
static unsigned renderer_flags = MD_HTML_FLAG_DEBUG | MD_HTML_FLAG_SKIP_UTF8_BOM;
#else
static unsigned renderer_flags = MD_HTML_FLAG_DEBUG;
#endif
static int want_fullhtml = 1;
static int want_xhtml = 0;
// static int want_stat = 0;


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
    if(buf->data == NULL) {
        fprintf(stderr, "membuf_init: malloc() failed.\n");
        exit(1);
    }
}

static void
membuf_fini(struct membuffer* buf)
{
    if(buf->data)
        free(buf->data);
}

static void
membuf_grow(struct membuffer* buf, size_t new_asize)
{
    buf->data = (char*)realloc(buf->data, new_asize);
    if(buf->data == NULL) {
        fprintf(stderr, "membuf_grow: realloc() failed.\n");
        exit(1);
    }
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

static int
process_file(FILE* in, FILE* out)
{
    size_t n;
    struct membuffer buf_in = {};
    struct membuffer buf_out = {};
    int ret = -1;
    // clock_t t0, t1;

    membuf_init(&buf_in, 32 * 1024);

    /* Read the input file into a buffer. */
    while(1) {
        if(buf_in.size >= buf_in.asize)
            membuf_grow(&buf_in, buf_in.asize + buf_in.asize / 2);

        n = fread(buf_in.data + buf_in.size, 1, buf_in.asize - buf_in.size, in);
        if(n == 0)
            break;
        buf_in.size += n;
    }

    /* Input size is good estimation of output size. Add some more reserve to
     * deal with the HTML header/footer and tags. */
    membuf_init(&buf_out, (MD_SIZE)(buf_in.size + buf_in.size/8 + 64));

    /* Parse the document. This shall call our callbacks provided via the
     * md_renderer_t structure. */
    // t0 = clock();

    ret = md_html(buf_in.data, (MD_SIZE)buf_in.size, process_output, (void*) &buf_out,
                  parser_flags, renderer_flags);

    // t1 = clock();
    if(ret != 0) {
        fprintf(stderr, "Parsing failed.\n");
        goto out;
    }

    /* Write down the document in the HTML format. */
    if(want_fullhtml) {
        if(want_xhtml) {
            fprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            fprintf(out, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" "
                         "\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n");
            fprintf(out, "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
        } else {
            fprintf(out, "<!DOCTYPE html>\n");
            fprintf(out, "<html>\n");
        }
        fprintf(out, "<head>\n");
        fprintf(out, "<title></title>\n");
        fprintf(out, "<meta name=\"generator\" content=\"md2html\"%s>\n", want_xhtml ? " /" : "");
        fprintf(out, "</head>\n");
        fprintf(out, "<body>\n");
    }

    fwrite(buf_out.data, 1, buf_out.size, out);

    if(want_fullhtml) {
        fprintf(out, "</body>\n");
        fprintf(out, "</html>\n");
    }

    /*if(want_stat) {
        if(t0 != (clock_t)-1  &&  t1 != (clock_t)-1) {
            double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;
            if (elapsed < 1)
                fprintf(stderr, "Time spent on parsing: %7.2f ms.\n", elapsed*1e3);
            else
                fprintf(stderr, "Time spent on parsing: %6.3f s.\n", elapsed);
        }
    }*/

    /* Success if we have reached here. */
    ret = 0;

out:
    membuf_fini(&buf_in);
    membuf_fini(&buf_out);

    return ret;
}


static const char* input_path = NULL;
static const char* output_path = NULL;


QString Parser::Parse(QString markdown, bool ok)
{
    FILE* in = stdin;
    FILE* out = stdout;
    int ret = 0;

    if(input_path != NULL && strcmp(input_path, "-") != 0) {
        in = fopen(input_path, "rb");
        if(in == NULL) {
            ok = false;
            return QString("Cannot open %1.").arg(input_path);
        }
    }
    if(output_path != NULL && strcmp(output_path, "-") != 0) {
        out = fopen(output_path, "wt");
        if(out == NULL) {
            ok = false;
            return QString("Cannot open %1.").arg(output_path);
        }
    }

    ret = process_file(in, out);
    if(in != stdin)
        fclose(in);
    if(out != stdout)
        fclose(out);

    ok = (ret == 0);
    Q_UNUSED(ok);
    return QString();
}
