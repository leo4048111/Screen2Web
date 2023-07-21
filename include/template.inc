#pragma once

const char *html_code_buf = "<!DOCTYPE html>\n"
                            "<html>\n"
                            "<head>\n"
                            "  <title>Screen2Web</title>\n"
                            "</head>\n"
                            "<body>\n"
                            "  <div><image src=\"\" id=\"imageContainer\"></image></div>\n"
                            "\n"
                            "  <script>\n"
                            "    function loadImage() {\n"
                            "      var xhr = new XMLHttpRequest();\n"
                            "      xhr.onreadystatechange = function() {\n"
                            "        if (xhr.readyState === 4 && xhr.status === 200) {\n"
                            "          const width = xhr.getResponseHeader('Image-Width');\n"
                            "          const height = xhr.getResponseHeader('Image-Height');\n"
                            "          var img = document.getElementById('imageContainer');\n"
const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');

        // 创建ImageData对象，用于存储像素数据
        const imageData = ctx.createImageData(width, height);

        // 将裸RGBA数据写入到ImageData对象中
        for (let i = 0; i < rawRgbaData.length; i += 4) {
            const pixelIndex = i / 4;
            imageData.data[pixelIndex * 4] = rawRgbaData[i]; // Red component
            imageData.data[pixelIndex * 4 + 1] = rawRgbaData[i + 1]; // Green component
            imageData.data[pixelIndex * 4 + 2] = rawRgbaData[i + 2]; // Blue component
            imageData.data[pixelIndex * 4 + 3] = rawRgbaData[i + 3]; // Alpha component
        }

        // 将ImageData对象绘制到Canvas上
        ctx.putImageData(imageData, 0, 0);
                            "\n"
                            "          loadImage();\n"
                            "        }\n"
                            "      };\n"
                            "\n"
                            "      xhr.open('GET', '/GetOneFrame', true);\n"
                            "      xhr.send();\n"
                            "    }\n"
                            "\n"
                            "    window.onload = function() {\n"
                            "      loadImage();\n"
                            "    };\n"
                            "  </script>\n"
                            "</body>\n"
                            "</html>";
