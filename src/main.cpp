#include <stdio.h>
#include <string.h>
#include <libxml/SAX.h>
#include <string>
 
int read_xmlfile(FILE *f);
xmlSAXHandler make_sax_handler();
 
static void OnStartElementNs(
    void *ctx,
    const xmlChar *localname,
    const xmlChar *prefix,
    const xmlChar *URI,
    int nb_namespaces,
    const xmlChar **namespaces,
    int nb_attributes,
    int nb_defaulted,
    const xmlChar **attributes
);
 
static void OnEndElementNs(
    void* ctx,
    const xmlChar* localname,
    const xmlChar* prefix,
    const xmlChar* URI
);
 
static void OnCharacters(void* ctx, const xmlChar * ch, int len);

static void OnAttributeDecl(void *ctx,
				const xmlChar *elem,
				const xmlChar *fullname,
				int type,
				int def,
				const xmlChar *defaultValue,
				xmlEnumerationPtr tree);
 
int main(int argc, char *argv[]) {
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        puts("file open error.");
        exit(1);
    }
 
    if(read_xmlfile(f)) {
        puts("xml read error.");
        exit(1);
    }
 
    fclose(f);
    return 0;
}
 
int read_xmlfile(FILE *f) {
    char chars[1024];
    int res = fread(chars, 1, 4, f);
    if (res <= 0) {
        return 1;
    }
 
    xmlSAXHandler SAXHander = make_sax_handler();
 
    xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt(
        &SAXHander, NULL, chars, res, NULL
    );
 
    while ((res = fread(chars, 1, sizeof(chars), f)) > 0) {
        if(xmlParseChunk(ctxt, chars, res, 0)) {
            xmlParserError(ctxt, "xmlParseChunk");
            return 1;
        }
    }
    xmlParseChunk(ctxt, chars, 0, 1);
 
    xmlFreeParserCtxt(ctxt);
    xmlCleanupParser();
    return 0;
}
 
xmlSAXHandler make_sax_handler (){
    xmlSAXHandler SAXHander;
 
    memset(&SAXHander, 0, sizeof(xmlSAXHandler));
 
    SAXHander.initialized = XML_SAX2_MAGIC;
    SAXHander.startElementNs = OnStartElementNs;
    SAXHander.endElementNs = OnEndElementNs;
    SAXHander.characters = OnCharacters;
    SAXHander.attributeDecl = OnAttributeDecl;
 
    return SAXHander;
}
 
static void OnStartElementNs(
    void *ctx,
    const xmlChar *localname,
    const xmlChar *prefix,
    const xmlChar *URI,
    int nb_namespaces,
    const xmlChar **namespaces,
    int nb_attributes,
    int nb_defaulted,
    const xmlChar **attributes
) {
    printf("<%s>\n", localname);
    unsigned int index = 0;
    for ( int indexAttribute = 0; 
            indexAttribute < nb_attributes; 
            ++indexAttribute, index += 5 ){
         const xmlChar *attrlocalname = attributes[index];
         const xmlChar *attrprefix = attributes[index+1];
         const xmlChar *attrnsURI = attributes[index+2];
         const xmlChar *valueBegin = attributes[index+3];
         const xmlChar *valueEnd = attributes[index+4];
         std::string value( (const char *)valueBegin, (const char *)valueEnd );
         printf( "  %sattribute: localname='%s', prefix='%s', uri=(%p)'%s', value='%s'\n",
                 indexAttribute >= (nb_attributes - nb_defaulted) ? "defaulted " : "",
                 attrlocalname,
                 attrprefix,
                 attrnsURI,
                 attrnsURI,
                 value.c_str() );        
    }
}

static void OnAttributeDecl(void *ctx,
				const xmlChar *elem,
				const xmlChar *fullname,
				int type,
				int def,
				const xmlChar *defaultValue,
				xmlEnumerationPtr tree)
{
    printf("OnAttr name='%s', val='%s'\n", fullname, defaultValue);
}

static void OnEndElementNs(
    void* ctx,
    const xmlChar* localname,
    const xmlChar* prefix,
    const xmlChar* URI
) {
    printf("</%s>\n", localname);
}
 
static void OnCharacters(void *ctx, const xmlChar *ch, int len) {
    char chars[1000 + 1];
    strncpy(chars, (const char *)ch, len);
    chars[len] = (char)NULL;
    printf("[%s]\n", chars);
}