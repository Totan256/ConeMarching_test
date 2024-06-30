
int readShaderSource(GLuint shaderObj, std::string fileName)
{
    //ファイルの読み込み
    std::ifstream ifs(fileName);
    if (!ifs)
    {
        std::cout << "error" << std::endl;
        return -1;
    }

    std::string source;
    std::string line;
    while (getline(ifs, line))
    {
        source += line + "\n";
    }

    // シェーダのソースプログラムをシェーダオブジェクトへ読み込む
    const GLchar* sourcePtr = (const GLchar*)source.c_str();
    GLint length = source.length();
    glShaderSource(shaderObj, 1, &sourcePtr, &length);

    return 0;
}

GLint makeShader(std::string vertexFileName, std::string fragmentFileName, std::string pv, std::string fc)
{
    // シェーダーオブジェクト作成
    GLuint vertShaderObj = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint shader;

    // シェーダーコンパイルとリンクの結果用変数
    GLint compiled, linked;

    /* シェーダーのソースプログラムの読み込み */
    if (readShaderSource(vertShaderObj, vertexFileName)) return -1;
    if (readShaderSource(fragShaderObj, fragmentFileName)) return -1;

    /* バーテックスシェーダーのソースプログラムのコンパイル */
    glCompileShader(vertShaderObj);
    glGetShaderiv(vertShaderObj, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE)
    {
        //fprintf(stderr, "Compile error in vertex shader.\n");
        std::cout << "Compile error in vertex shader.\n" << std::endl;
        GLsizei bufSize;
        glGetShaderiv(vertShaderObj, GL_INFO_LOG_LENGTH, &bufSize);
        if (bufSize > 1) {
            GLchar *infoLog = new GLchar[bufSize];
            GLsizei length;
            glGetShaderInfoLog(vertShaderObj, bufSize, &length, infoLog);
            std::cerr << infoLog << std::endl;
            delete[] infoLog;
        }
        return -1;
    }

    /* フラグメントシェーダーのソースプログラムのコンパイル */
    glCompileShader(fragShaderObj);
    glGetShaderiv(fragShaderObj, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE)
    {
        std::cout << "Compile error in fragment shader.\n" << std::endl;
        GLsizei bufSize;
        glGetShaderiv(fragShaderObj, GL_INFO_LOG_LENGTH, &bufSize);
        if (bufSize > 1) {
            GLchar* infoLog = new GLchar[bufSize];
            GLsizei length;
            glGetShaderInfoLog(fragShaderObj, bufSize, &length, infoLog);
            std::cerr << infoLog << std::endl;
            delete[] infoLog;
        }
        return -1;
    }

    /* プログラムオブジェクトの作成 */
    shader = glCreateProgram();

    /* シェーダーオブジェクトのシェーダープログラムへの登録 */
    glAttachShader(shader, vertShaderObj);
    glAttachShader(shader, fragShaderObj);

    /* シェーダーオブジェクトの削除 */
    glDeleteShader(vertShaderObj);
    glDeleteShader(fragShaderObj);

    /* シェーダ変数バインド*/
    glBindAttribLocation(shader, 0, pv.c_str());
    glBindFragDataLocation(shader, 0, fc.c_str());

    /* シェーダープログラムのリンク */
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        
        std::cout << "Link error.\n" << std::endl;
        GLsizei bufSize;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

        if (bufSize > 1)
        {
            // シェーダのリンク時のログの内容を取得する
            GLchar* infoLog = new GLchar[bufSize];
            GLsizei length;
            glGetProgramInfoLog(shader, bufSize, &length, infoLog);
            std::cerr << infoLog << std::endl;
            delete[] infoLog;
        }

        return -1;
    }

    return shader;
}
