
int readShaderSource(GLuint shaderObj, std::string fileName)
{
    //�t�@�C���̓ǂݍ���
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

    // �V�F�[�_�̃\�[�X�v���O�������V�F�[�_�I�u�W�F�N�g�֓ǂݍ���
    const GLchar* sourcePtr = (const GLchar*)source.c_str();
    GLint length = source.length();
    glShaderSource(shaderObj, 1, &sourcePtr, &length);

    return 0;
}

GLint makeShader(std::string vertexFileName, std::string fragmentFileName, std::string pv, std::string fc)
{
    // �V�F�[�_�[�I�u�W�F�N�g�쐬
    GLuint vertShaderObj = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint shader;

    // �V�F�[�_�[�R���p�C���ƃ����N�̌��ʗp�ϐ�
    GLint compiled, linked;

    /* �V�F�[�_�[�̃\�[�X�v���O�����̓ǂݍ��� */
    if (readShaderSource(vertShaderObj, vertexFileName)) return -1;
    if (readShaderSource(fragShaderObj, fragmentFileName)) return -1;

    /* �o�[�e�b�N�X�V�F�[�_�[�̃\�[�X�v���O�����̃R���p�C�� */
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

    /* �t���O�����g�V�F�[�_�[�̃\�[�X�v���O�����̃R���p�C�� */
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

    /* �v���O�����I�u�W�F�N�g�̍쐬 */
    shader = glCreateProgram();

    /* �V�F�[�_�[�I�u�W�F�N�g�̃V�F�[�_�[�v���O�����ւ̓o�^ */
    glAttachShader(shader, vertShaderObj);
    glAttachShader(shader, fragShaderObj);

    /* �V�F�[�_�[�I�u�W�F�N�g�̍폜 */
    glDeleteShader(vertShaderObj);
    glDeleteShader(fragShaderObj);

    /* �V�F�[�_�ϐ��o�C���h*/
    glBindAttribLocation(shader, 0, pv.c_str());
    glBindFragDataLocation(shader, 0, fc.c_str());

    /* �V�F�[�_�[�v���O�����̃����N */
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        
        std::cout << "Link error.\n" << std::endl;
        GLsizei bufSize;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

        if (bufSize > 1)
        {
            // �V�F�[�_�̃����N���̃��O�̓��e���擾����
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
