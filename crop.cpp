//
// Created by huihui on 2023/6/3.
//

#include <iostream>
#include <string>
#include "glad/gl.h"
#include <GLFW/glfw3.h>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

// 顶点位置数组
float vertices[] = {
        // Positions
        -1.0f, 1.0f, 0.0f,  // 左上角
        1.0f, 1.0f, 0.0f,  // 右上角
        -1.0f, -1.0f, 0.0f,  // 左下角
        1.0f, -1.0f, 0.0f   // 右下角
};

//// 纹理位置数组
float tex_coords[] = {
        // Texture Coordinates
        0.0f, 0.5f,  // 左上角
        0.5f, 0.5f,  // 右上角
        0.0f, 0.0f,  // 左下角
        0.5f, 0.0f   // 右下角
};

// 索引数组
unsigned int indices[] = {
        0, 1, 2,  // 第一个三角形
        1, 3, 2   // 第二个三角形
};


float asp1 = 16.0f / 9.0f;  // 屏幕宽高比
float asp2 = 4.0f / 3.0f;   // 图片宽高比

// 窗口大小回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 处理输入事件函数
void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(800, 600, "CenterCrop Example", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }
    // 设定视口大小和位置
    glViewport(0, 0, 800, 600);
    asp1 = 8.0f / 6.0f;
    // 注册窗口大小改变事件回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 创建着色器程序
    const char *vertex_shader_source = R"(#version 330 core
                                       layout (location = 0) in vec3 aPos;
                                       layout (location = 1) in vec2 aTexCoord;
                                       out vec2 TexCoord;
                                       void main() {
                                           gl_Position = vec4(aPos, 1.0);
                                           TexCoord = aTexCoord;
                                       })";
    const char *fragment_shader_source = R"(#version 330 core
                                         out vec4 FragColor;
                                         in vec2 TexCoord;
                                         uniform sampler2D tex;
                                         void main() {
                                             FragColor = texture(tex, TexCoord);
                                         })";
    unsigned int vertex_shader, fragment_shader, shader_program;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    // 检查编译结果
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "Failed to compile vertex shader! " << info_log << std::endl;
        return -1;
    }
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "Failed to compile fragment shader! " << info_log << std::endl;
        return -1;
    }
    // 创建着色器程序并链接
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    // 检查链接结果
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cout << "Failed to link shader program! " << info_log << std::endl;
        return -1;
    }
    // 删除着色器对象
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // 加载图片
    int image_width, image_height, image_channels;
    stbi_set_flip_vertically_on_load(true);
//    unsigned char *image_data = stbi_load("pic1.jpg", &image_width, &image_height, &image_channels, 0);
    unsigned char *image_data = stbi_load("mesh.png", &image_width, &image_height, &image_channels, 0);
    if (image_data == NULL) {
        std::cout << "Failed to load image!" << std::endl;
        return -1;
    }
//    std::ofstream out_file("output.rgb", std::ios::binary | std::ios::out);
//    out_file.write(reinterpret_cast<const char *>(image_data), image_height * image_width * image_channels);
//    out_file.close();

    printf("image : %dx%d\n", image_width, image_height);
    asp2 = float(image_width) / float(image_height);
    printf("asp1 : %f asp2 %f\n", asp1, asp2);
    // 计算裁剪后的图片纹理坐标
    float aspect_ratio = (asp1 < asp2) ? (asp1 / asp2) : (asp2 / asp1);
    float crop_width = (asp1 < asp2) ? aspect_ratio : 1.0f;
    float crop_height = (asp1 > asp2) ?  aspect_ratio : 1.0f;
    float crop_x = (1.0f - crop_width) / 2.0f;
    float crop_y = (1.0f - crop_height) / 2.0f;
    float tex_coords[] = {
            crop_x, crop_y + crop_height,  // 左上角
            crop_x + crop_width, crop_y + crop_height,  // 右上角
            crop_x, crop_y,  // 左下角
            crop_x + crop_width, crop_y  // 右下角
    };

    // 创建纹理对象
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载图片数据到纹理中
    GLenum format = (image_channels == 4) ? GL_RGBA : GL_RGB;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image_width, image_height, 0, format, GL_UNSIGNED_BYTE, image_data);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    // 生成多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);
    // 释放图片数据
    stbi_image_free(image_data);
    // 创建顶点数组对象、顶点缓冲对象和索引缓冲对象
    unsigned int VAO, VBO, TBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &TBO);
    glGenBuffers(1, &EBO);

    // 绑定顶点数组对象
    glBindVertexArray(VAO);
    // 绑定 VBO，并复制顶点数据到缓冲中
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);

    // 创建 EBO，并复制索引数据到缓冲中
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 创建纹理坐标缓冲对象
    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);

    // 设置纹理坐标属性指针
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(1);

    // 解绑 VAO，VBO，TBO 和 EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入事件
        process_input(window);

        // 清空颜色缓冲
        glClearColor(0.2f, 0.3f,
                     0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 激活纹理单元，绑定纹理对象到当前激活的纹理单元
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        // 将纹理单元设置给 uniform 变量
        glUniform1i(glGetUniformLocation(shader_program, "tex"), 0);

        // 绘制矩形
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // 交换颜色缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 删除 VAO，VBO，TBO 和 EBO
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &TBO);
    glDeleteBuffers(1, &EBO);
    // 删除纹理对象
    glDeleteTextures(1, &texture);
    // 删除着色器程序
    glDeleteProgram(shader_program);

    // 终止 GLFW
    glfwTerminate();
    return 0;
}
