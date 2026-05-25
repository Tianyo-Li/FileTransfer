#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12346
#define BUFFER_SIZE 65536

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "用法: " << argv[0] << " <服务器IP> <文件名>\n";
        std::cerr << "示例: " << argv[0] << " 10.0.2.2 test.txt\n";
        return 1;
    }
    
    const char* server_ip = argv[1];
    const char* filename = argv[2];
    
    // 打开文件
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return 1;
    }
    
    // 获取文件大小
    infile.seekg(0, std::ios::end);
    long long filesize = infile.tellg();
    infile.seekg(0, std::ios::beg);
    
    std::cout << "文件: " << filename << ", 大小: " << filesize << " 字节" << std::endl;
    
    // 创建socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket创建失败" << std::endl;
        return 1;
    }
    
    // 配置服务器地址（Qt客户端）
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    
    // 连接Qt客户端
    std::cout << "正在连接 " << server_ip << ":" << PORT << " ..." << std::endl;
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "连接失败" << std::endl;
        return 1;
    }
    
    std::cout << "连接成功！开始发送文件..." << std::endl;
    
    // 发送文件头: FILE:文件名|文件大小\n
    char header[512];
    snprintf(header, sizeof(header), "FILE:%s|%lld\n", filename, filesize);
    send(sock, header, strlen(header), 0);
    
    // 发送文件内容
    char buffer[BUFFER_SIZE];
    long long sent = 0;
    int last_percent = -1;
    
    while (sent < filesize) {
        int to_read = (filesize - sent) < BUFFER_SIZE ? (filesize - sent) : BUFFER_SIZE;
        infile.read(buffer, to_read);
        int n = infile.gcount();
        if (n <= 0) break;
        
        send(sock, buffer, n, 0);
        sent += n;
        
        int percent = (int)(sent * 100 / filesize);
        if (percent != last_percent) {
            std::cout << "\r进度: " << percent << "% [" << sent << "/" << filesize << "]" << std::flush;
            last_percent = percent;
        }
    }
    
    infile.close();
    close(sock);
    std::cout << "\n发送完成！" << std::endl;
    return 0;
}