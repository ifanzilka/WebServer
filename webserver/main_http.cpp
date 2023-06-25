#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class HttpRequest
{
public:
    std::string method;
    std::string path;
    std::string http_version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class HttpParser
{
public:
    HttpParser() : state_(Method) {}

    bool parse(char c) {
        switch (state_) {
            case Method:
                if (c == ' ') {
                    state_ = Path;
                } else {
                    request_.method.push_back(c);
                }
                break;
            case Path:
                if (c == ' ') {
                    state_ = HttpVersion;
                } else {
                    request_.path.push_back(c);
                }
                break;
            case HttpVersion:
                if (c == 'r') {
                    state_ = ExpectingNewline1;
                } else {
                    request_.http_version.push_back(c);
                }
                break;
            case ExpectingNewline1:
                if (c == 'n') {
                    state_ = HeaderLineStart;
                } else {
                    return false;
                }
                break;
            case HeaderLineStart:
                if (c == 'r') {
                    state_ = ExpectingNewline3;
                } else if (!request_.headers.empty() && (c == ' ' || c == 't')) {
                    state_ = HeaderLws;
                } else {
                    state_ = HeaderName;
                    current_header_name_.push_back(c);
                }
                break;
            case HeaderLws:
                if (c == 'r') {
                    state_ = ExpectingNewline2;
                } else if (c == ' ' || c == 't') {
                    // Ignore whitespace
                } else {
                    state_ = HeaderValue;
                    current_header_value_.push_back(c);
                }
                break;
            case HeaderName:
                if (c == ':') {
                    state_ = SpaceBeforeHeaderValue;
                } else {
                    current_header_name_.push_back(c);
                }
                break;
            case SpaceBeforeHeaderValue:
                if (c == ' ') {
                    state_ = HeaderValue;
                } else {
                    return false;
                }
                break;
            case HeaderValue:
                if (c == 'r') {
                    request_.headers[current_header_name_] = current_header_value_;
                    current_header_name_.clear();
                    current_header_value_.clear();
                    state_ = ExpectingNewline2;
                } else {
                    current_header_value_.push_back(c);
                }
                break;
            case ExpectingNewline2:
                if (c == 'n') {
                    state_ = HeaderLineStart;
                } else {
                    return false;
                }
                break;
            case ExpectingNewline3:
                if (c == 'n') {
                    state_ = Body;
                } else {
                    return false;
                }
                break;
            case Body:
                request_.body.push_back(c);
                break;
        }
        return true;
    }

    bool is_done() const {
        return state_ == Body;
    }

    HttpRequest get_request() const {
        return request_;
    }

private:
    enum State {
        Method,
        Path,
        HttpVersion,
        ExpectingNewline1,
        HeaderLineStart,
        HeaderLws,
        HeaderName,
        SpaceBeforeHeaderValue,
        HeaderValue,
        ExpectingNewline2,
        ExpectingNewline3,
        Body,
    };

    State state_;
    HttpRequest request_;
    std::string current_header_name_;
    std::string current_header_value_;
};

int main() {
    HttpParser parser;
    std::string input = "GET / HTTP/1.1"\
        "Host: localhost:4242"\
        "Connection: keep-alive"\
        "Cache-Control: max-age=0"\
        "sec-ch-ua: \"Google Chrome\";v=\"113\", \"Chromium\";v=\"113\", \"Not-A.Brand\";v=\"24\""\
        "sec-ch-ua-mobile: ?0 \""
        "sec-ch-ua-platform: \"macOS\""
        "Upgrade-Insecure-Requests: 1"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7"
        "Sec-Fetch-Site: none"
        "Sec-Fetch-Mode: navigate"
        "Sec-Fetch-User: ?1"
        "Sec-Fetch-Dest: document"
        "Accept-Encoding: gzip, deflate, br"
        "Accept-Language: ru,ru-RU;q=0.9,en-US;q=0.8,en;q=0.7;\"";
        std::cout << input << std::endl;
    // for (char c : input) {
    //     if (!parser.parse(c)) {
    //         std::cout << "Error parsing input" << std::endl;
    //         return 1;
    //     }
    // }
    // if (!parser.is_done()) {
    //     std::cout << "Incomplete input" << std::endl;
    //     return 1;
    // }
    HttpRequest request = parser.get_request();
    std::cout << "Method: " << request.method << std::endl;
    std::cout << "Path: " << request.path << std::endl;
    std::cout << "HTTP version: " << request.http_version << std::endl;
    std::cout << "Headers:" << std::endl;
    for (const auto& header : request.headers) {
        std::cout << "  " << header.first << ": " << header.second << std::endl;
    }
    std::cout << "Body: " << request.body << std::endl;
    return 0;
}
