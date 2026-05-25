#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define BUFFER_SIZE 65536  // 64KB 缓冲区
#define TIMEOUT_SEC 30      // 30秒超时

int main(int argc, char *argv[]) {
    // 检查命令行参数
    if (argc < 2) {
        std::cerr << "用法: " << argv[0] << " <服务器IP地址>" << std::endl;
        std::cerr << "示例: " << argv[0] << " 10.0.2.2" << std::endl;
        return 1;
    }
    
    const char* server_ip = argv[1];
    std::cout << "正在连接服务器 " << server_ip << ":" << PORT << " ..." << std::endl;
    
    // 创建 socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket 创建失败" << std::endl;
        return 1;
    }
    
    // 设置超时
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    // 配置服务器地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    
    // 连接服务器
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "连接失败: " << strerror(errno) << std::endl;
        close(sock);
        return 1;
    }
    
    std::cout << "✓ 连接成功！等待接收文件..." << std::endl;
    
    // 接收文件头（格式: "FILE:文件名|文件大小\n"）
    char buffer[BUFFER_SIZE];
    int n = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    
    if (n <= 0) {
        std::cerr << "接收文件头失败: " << (n == 0 ? "连接已关闭" : strerror(errno)) << std::endl;
        close(sock);
        return 1;
    }
    
    buffer[n] = '\0';
    std::string header(buffer);
    
    // 解析文件头
    if (header.substr(0, 5) != "FILE:") {
        std::cerr << "无效的协议格式，收到: " << header.substr(0, 50) << std::endl;
        close(sock);
        return 1;
    }
    
    size_t pipe_pos = header.find('|');
    if (pipe_pos == std::string::npos) {
        std::cerr << "文件头格式错误" << std::endl;
        close(sock);
        return 1;
    }
    
    std::string filename = header.substr(5, pipe_pos - 5);
    long long filesize = std::stoll(header.substr(pipe_pos + 1));
    
    std::cout << "正在接收文件: " << filename << std::endl;
    std::cout << "文件大小: " << filesize << " 字节" << std::endl;
    
    // 打开文件准备写入
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "无法创建文件: " << filename << std::endl;
        close(sock);
        return 1;
    }
    
    // 接收文件数据
    long long received = 0;
    int last_percent = -1;
    
    while (received < filesize) {
        long long remaining = filesize - received;
        int to_read = (remaining < BUFFER_SIZE) ? remaining : BUFFER_SIZE;
        
        n = recv(sock, buffer, to_read, 0);
        if (n <= 0) {
            if (n == 0) {
                std::cerr << "\n连接意外关闭" << std::endl;
            } else {
                std::cerr << "\n接收失败: " << strerror(errno) << std::endl;
            }
            break;
        }
        
        outfile.write(buffer, n);
        received += n;
        
        // 显示进度（每 5% 或收到足够数据时更新）
        int percent = (int)(received * 100 / filesize);
        if (percent != last_percent) {
            std::cout << "\r进度: " << percent << "% [" << received << "/" << filesize << "]" << std::flush;
            last_percent = percent;
        }
    }
    
    outfile.close();
    close(sock);
    
    if (received == filesize) {
        std::cout << "\n✓ 文件接收成功！保存为: " << filename << std::endl;
    } else {
        std::cerr << "\n✗ 文件接收不完整: " << received << "/" << filesize << " 字节" << std::endl;
        return 1;
    }
    
    return 0;
}