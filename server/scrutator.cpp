// Inclure les headers Apache dans extern "C"
extern "C" {
#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_request.h"
#include "ap_config.h"
}

#include <string>
#include <windows.h>

std::string wide_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// --- fonction utilitaire : dernier fichier modifié ---
std::wstring get_last_modified_file(const std::wstring& directory) {
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    std::wstring searchPath = directory + L"\\*";
    FILETIME lastWriteTime = { 0, 0 };
    std::wstring lastFile;

    hFind = FindFirstFileW(searchPath.c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return L"";
    }

    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (CompareFileTime(&lastWriteTime, &findFileData.ftLastWriteTime) < 0) {
                lastWriteTime = findFileData.ftLastWriteTime;
                lastFile = findFileData.cFileName;
            }
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);
    return lastFile;
}

// --- handler HTTP ---
extern "C" static int scrutator_handler(request_rec* r) {
    if (!r->handler || strcmp(r->handler, "scrutator")) {
        return DECLINED;
    }

    r->content_type = "text/plain";

    if (r->method_number == M_GET) {
        const std::wstring folder = L"C:\\Apache24\\htdocs\\holo3";
        ap_set_content_type(r, "text/event-stream;charset=UTF-8");
        apr_table_set(r->headers_out, "Cache-Control", "no-cache");
        apr_table_set(r->headers_out, "Connection", "keep-alive");
        apr_table_set(r->headers_out, "Access-Control-Allow-Origin", "*"); // utile si cross-origin

        std::wstring file = get_last_modified_file(folder);

        if (!file.empty()) {
            std::string utf8file = wide_to_utf8(file);
            ap_rprintf(r, "event: file-added\ndata: {\"file\":\"%s\"}\n\n", utf8file.c_str());
            ap_rflush(r); // FORCE l’envoi immédiat
        }
        else {
            ap_set_content_type(r, "text/event-stream;charset=UTF-8");
        }
        return OK;
    }

    return HTTP_METHOD_NOT_ALLOWED;
}

// --- déclaration du module ---
static void scrutator_register_hooks(apr_pool_t* p) {
    ap_hook_handler(scrutator_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

extern "C" {
    module AP_MODULE_DECLARE_DATA scrutator_module = {
        STANDARD20_MODULE_STUFF,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        scrutator_register_hooks
    };
}