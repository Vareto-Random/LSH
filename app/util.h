#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <functional>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

int bin2int(const cv::Mat bin) {

    assert(bin.rows == 1);
    int r = 0;
    for(int c = 0; c < bin.cols; ++c)
        r += bin.at<uchar>(0, c) << c;
    return r;
}

void ls(const std::string &path,
        std::function<void(const boost::filesystem::directory_iterator&) > f) {

    namespace fs = boost::filesystem;

    fs::path p(path);
    fs::directory_iterator end_iter;

    if (fs::exists(p) && fs::is_directory(p))
        for (fs::directory_iterator dir_iter(p); dir_iter != end_iter; ++dir_iter)
            f(dir_iter);
}

std::vector<std::string> ls_files(const std::string &path) {

    std::vector<std::string> output;

    namespace fs = boost::filesystem;

    ls(path, [&](const fs::directory_iterator it) {
        if (fs::is_regular_file(it->status()))
            output.push_back(it->path().string());
    });

    std::sort(output.begin(), output.end());

    return output;
}

std::vector<std::string> ls_folders(const std::string &path) {

    std::vector<std::string> output;

    namespace fs = boost::filesystem;

    ls(path, [&](const fs::directory_iterator it) {
        if (fs::is_directory(it->status()))
            output.push_back(it->path().string());
    });

    std::sort(output.begin(), output.end());

    return output;
}

std::string path_filename(const std::string &path) {

    namespace fs = boost::filesystem;
    fs::path p(path);
    return p.filename().string();
}

class HTML {
public:
    std::ofstream output;

    HTML(const std::string &filename)
    : output(filename) {

        output << "<!DOCTYPE html>" << '\n';
        output << "<html lang=\"en\">" << '\n';
        output << "<head>" << '\n';
        output << "  <title>Ranks</title>" << '\n';
        output << "  <meta charset=\"utf-8\">" << '\n';
        output << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << '\n';
        output << "  <link rel=\"stylesheet\" href=\"bootstrap/css/bootstrap.min.css\">" << '\n';
        output << "  <script src=\"bootstrap/js/bootstrap.min.js\"></script>" << '\n';
        output << "</head>" << '\n';
        output << "<body>" << '\n';
        output << "<div class=\"content\">" << '\n';
    }

    ~HTML() {
        output << "</body>" << '\n';
        output << "</div>" << '\n';
        output << "</html>" << '\n';
        output.close();
    }

    void write(const std::string query,
            const std::vector<std::pair<std::string, float>> &l) {

        output << "<div class=\"panel panel-default\">" << '\n';
        output << "<div class=\"panel-heading\">" << query << "</div>" << '\n';
        output << "<table class=\"table\">" << '\n';

        output << "<thead>" << '\n';
        output << "<tr>" << '\n';
        output << "<th>Query</th>" << '\n';
        for (int r = 0; r < std::min(10, (int) l.size()); ++r)
            output << "<th>Rank " << std::to_string(r + 1) << "</th>" << '\n';
        output << "</tr>" << '\n';
        output << "</thead>" << '\n';

        output << "<tbody>" << '\n';
        output << "<tr>" << '\n';
        output << "<td class=\"alert-info\">"
                "<a href=\"#\" class=\"thumbnail\">"
                "<img src=\"" << query << "\" class=\"img-thumbnail\" width=\"64\" height=\"64\"/>"
                "<div class=\"caption\">"
                "<h4 style=\"text-align: center\">" << "query" << "</h4>"
                "</div></a></td>" << '\n';
        for (int r = 0; r < std::min(10, (int) l.size()); ++r)
            output << "<td><a href=\"#\" class=\"thumbnail\">"
                "<img src=\"" << l[r].first << "\" class=\"img-thumbnail\" width=\"64\" height=\"64\"/>"
                "<div class=\"caption\"><h4 style=\"text-align: center\">" << l[r].second << "</h4></div>"
                "</a></td>" << '\n';
        output << "</tr>" << '\n';
        output << "</tbody>" << '\n';

        output << "</table>" << '\n';
        output << "</div>" << '\n';
    }
};

#endif // UTIL_H
