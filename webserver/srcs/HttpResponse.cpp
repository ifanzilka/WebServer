#include "HttpResponse.hpp"
#include "utils.hpp"

HttpResponse::HttpResponse()
{

} 

std::string HttpResponse::MakeHTTPResponse(int status, const std::string& content_type, const std::string& body)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " " << get_status_text(status) << "\r\n";
    oss << "Content-Type: " << content_type << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "\r\n";
    oss << body;
    std::string response_str = oss.str();
    //sendall(sockfd, response_str.c_str(), response_str.size());
    return (response_str);
}

std::string HttpResponse::MakeHTTPResponse(int status, const std::string& content_type, int size)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " " << get_status_text(status) << "\r\n";
    oss << "Content-Type: " << content_type << "\r\n";
    oss << "Content-Length: " << size << "\r\n";
    oss << "\r\n";

    std::string response_str = oss.str();
    //sendall(sockfd, response_str.c_str(), response_str.size());
    return (response_str);
}

int HttpResponse::SendHTTPResponseFile(int fd, int status, const std::string& content_type, const std::string& filename)
{

    std::string bytes;
    std::ifstream file(filename, std::ios::binary);
    std::ostringstream contents;
    contents << file.rdbuf();
    bytes = contents.str();

    std::ostringstream  oss;
    std::string         response;

    oss << "HTTP/1.1 " << status << " " << get_status_text(status) << "\r\n";
    //response = "HTTP/1.1 " + std::to_string(status) + get_status_text(status) + "\r\n";
    set_header(oss, "Content-Type", content_type);
    set_header(oss, "Content-Length", std::to_string(bytes.size()));
    set_header(oss, "Content-Disposition", "attachment; filename=\"" + filename + "\"");
    
    // oss << "Content-Type: " << content_type << "\r\n";
    // oss << "Content-Length: " << bytes.size() << "\r\n";
    // oss << "Content-Disposition: attachment; filename=\"" << filename << "\"\r\n";
    
    oss << "\r\n";
    //response += "r\n";

    response = oss.str();
    
    send_bytes_fd(fd, response.c_str(), response.size());
    send_bytes_fd(fd, bytes.c_str(), bytes.size());
    return (0);
}

std::string HttpResponse::SendBytesHTTPResponse(int fd, const std::string& filename)
{
    std::string bytes;

    std::ifstream file(filename, std::ios::binary);
    std::ostringstream contents;
    contents << file.rdbuf();
    bytes = contents.str();

    send_bytes_fd(fd, bytes.c_str(), bytes.size());

}

int send_bytes_fd(int fd, const char *msg, int size)
{
	char 	*c_msg = (char *)(const char *)msg;
	int 	res_send;
	
	size_t 	sended = 0;
	size_t 	len_msg = size;
	size_t  BUFFER_LEN = BUFFER_SIZE_SEND > size ? size : BUFFER_SIZE_SEND;
	int     cout_break = 0;

	while (sended < len_msg)
	{
		
		res_send = send(fd, c_msg, BUFFER_LEN , 0);
		//std::cout << (sended / 1024) / 1024  << "MB\n";
        if (res_send == -1)
		{
            int errno_code =  get_errno_code();

            if (errno_code == EAGAIN)
                continue;

            
            cout_break += 1;
			return (-1);
		}

		sended += res_send;
		c_msg += res_send;

	}
	return (0);
}

std::string HttpResponse::set_header(std::string &response , const std::string& key, const std::string& value)
{
    response += key;
    response += ": ";
    response += value;
    response += "\r\n";

    return (response);
}

std::ostringstream HttpResponse::set_header(std::ostringstream &response , const std::string& key, const std::string& value)
{
    std::ostringstream oss;
    response << key << ": " << value << "\r\n";

    return (oss);
}

std::string HttpResponse::get_status_text(int status)
{
    switch (status)
    {
        case 200:
            return "OK";
        case 400:
            return "Bad Request";
        case 404:
            return "Not Found";
        case 500:
            return "Internal Server Error";
        default:
            return "Unknown Status";
    }
}