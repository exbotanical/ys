/*
Addons:
- JSON
- Static File Server
- Session store?
- Reactive frontend framework!
- Styled components?
- Some sort of wasm bs :P
*/

// #define CONTENT_TYPE "Content-Type"
// #define CONTENT_LENGTH "Content-Length"
// #define LAST_MODIFIED "Last-Modified"

// #define CONTENT_TYPE_HTML "text/html"
// #define CONTENT_TYPE_TEXT "text/plain"
// #define CONTENT_TYPE_CSS "text/css"
// #define CONTENT_TYPE_JS "application/javascript"
// #define CONTENT_TYPE_IMAGE "image/jpeg"
// #define CONTENT_TYPE_UNKNOWN "application/octet-stream"

// typedef struct {
//   char *name;
//   char *path;
//   size_t size;
//   time_t mtime;
//   char *mime;
// } file_info;

// file_info *get_file_info(const char *root, const char *name) {
//   file_info *info = xmalloc(sizeof(file_info));
//   // Allocate memory for file path
//   char *path = xmalloc(strlen(root) + strlen(name) + 2);

//   // Construct file path
//   sprintf(path, "%s/%s", root, name);

//   // Get file information
//   struct stat st;
//   if (stat(path, &st) != 0) {
//     free(path);
//     return NULL;
//   }

//   // Check if file is a directory
//   if (S_ISDIR(st.st_mode)) {
//     free(path);
//     return NULL;
//   }

//   // Allocate memory for file info
//   info->name = s_copy(name);
//   info->path = path;
//   info->size = st.st_size;
//   info->mtime = st.st_mtime;

//   // Determine MIME type based on file extension
//   char *ext = strrchr(name, '.');
//   if (ext != NULL) {
//     if (s_casecmp(ext, ".html") || s_casecmp(ext, ".htm")) {
//       info->mime = CONTENT_TYPE_HTML;
//     } else if (s_casecmp(ext, ".txt")) {
//       info->mime = CONTENT_TYPE_TEXT;
//     } else if (s_casecmp(ext, ".css")) {
//       info->mime = CONTENT_TYPE_CSS;
//     } else if (s_casecmp(ext, ".js")) {
//       info->mime = CONTENT_TYPE_JS;
//     } else if (s_casecmp(ext, ".jpg") || s_casecmp(ext, ".jpeg")) {
//       info->mime = CONTENT_TYPE_IMAGE;
//     } else {
//       info->mime = CONTENT_TYPE_UNKNOWN;
//     }
//   } else {
//     info->mime = CONTENT_TYPE_UNKNOWN;
//   }

//   return info;
// }

// void free_file_info(file_info *info) {
//   free(info->name);
//   free(info->path);
// }

// void serve_file(request *req, response *res, const char *root,
//                 const char *name) {
//   // Get information about file
//   file_info *info = get_file_info(root, name);
//   if (info == NULL) {
//     set_status(res, STATUS_NOT_FOUND);
//     return;
//   }

//   // Check if file has been modified since last request
//   char *if_modified_since = get_header(req, "If-Modified-Since");
//   if (if_modified_since != NULL) {
//     struct tm tm;

//     if (strptime(if_modified_since, "%a, %d %b %Y %H:%M:%S GMT", &tm) !=
//     NULL) {
//       time_t if_modified_time = mktime(&tm);

//       if (if_modified_time >= info->mtime) {
//         set_status(res, STATUS_NOT_MODIFIED);
//         return;
//       }
//     }
//   }

//   // TODO: dont fast-fail and handle errors
//   char *bytes = from_file(info->path);
//   if (!bytes) {
//     set_status(res, STATUS_INTERNAL_SERVER_ERROR);
//     return;
//   }

//   set_header(res, "Content-Type", get_content_type(info->path));

//   char last_modified[32];
//   strftime(last_modified, sizeof(last_modified), "%a, %d %b %Y %H:%M:%S GMT",
//            gmtime(&info->mtime));
//   set_header(res, "Last-Modified", last_modified);

//   set_body(res, bytes);
// }

// void serve_dir(request *req, response *res, char *root_dir) {
//   char filepath[1024];
//   snprintf(filepath, sizeof(filepath), "%s/%s", root_dir, req->path);

//   struct stat st;
//   if (stat(filepath, &st) == -1) {
//     set_status(res, STATUS_NOT_FOUND);
//     return;
//   }

//   if (S_ISDIR(st.st_mode)) {
//     // If URL does not end with a slash, redirect to URL with slash
//     if (req->path[strlen(req->path) - 1] != '/') {
//       char location[1024];
//       snprintf(location, sizeof(location), "%s/", req->path);
//       set_status(res, STATUS_MOVED_PERMANENTLY);
//       set_header(res, "Location", location);

//       return;
//     }

//     // Serve index.html or index.htm file if present
//     char index_file[1024];
//     snprintf(index_file, sizeof(index_file), "%s/index.html", filepath);
//     if (stat(index_file, &st) == 0) {
//       serve_file(req, res, root_dir, index_file);
//       return;
//     }
//     snprintf(index_file, sizeof(index_file), "%s/index.htm", filepath);
//     if (stat(index_file, &st) == 0) {
//       serve_file(req, res, root_dir, index_file);
//       return;
//     }

//     // Serve directory listing
//     // serve_dir_listing(req, res, filepath); // TODO:
//     return;
//   }

//   // Serve file
//   serve_file(req, res, root_dir, filepath);
// }
