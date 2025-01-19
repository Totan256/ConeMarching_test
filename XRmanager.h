#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <vector>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

// �G���[�`�F�b�N�p�}�N��
#define XR_CHECK(result, msg) \
    if (XR_FAILED(result)) { \
        std::cerr << msg << std::endl; \
        return false; \
    }

bool CheckOpenXRisUsable() {
    XrInstance instance;
    XrSystemId systemId;
    XrSession session;
    XrResult result;

    // 1. OpenXR �C���X�^���X�̍쐬
    XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
    createInfo.applicationInfo = { "MyVRApp", 1, "CustomEngine", 1, XR_CURRENT_API_VERSION };
    result = xrCreateInstance(&createInfo, &instance);
    XR_CHECK(result, "Failed to create OpenXR instance");
    {
        XrInstanceProperties instProps = { XR_TYPE_INSTANCE_PROPERTIES };
        result = xrGetInstanceProperties(instance, &instProps);
        XR_CHECK(result, "Failed to get OpenXR instance properties");
        std::cout << "Runtime: " << instProps.runtimeName << std::endl;
        std::cout << "Version: " << XR_VERSION_MAJOR(instProps.runtimeVersion) 
            << "." << XR_VERSION_MINOR(instProps.runtimeVersion) 
            << "." << XR_VERSION_PATCH(instProps.runtimeVersion) << std::endl;
       
    }

    // 2. �V�X�e���̎擾
    XrSystemGetInfo systemInfo = { XR_TYPE_SYSTEM_GET_INFO };
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    result = xrGetSystem(instance, &systemInfo, &systemId);
    XR_CHECK(result, "Failed to get OpenXR system or HMD not found");

    // 3. �Z�b�V�����̍쐬
    XrSessionCreateInfo sessionCreateInfo = { XR_TYPE_SESSION_CREATE_INFO };
    sessionCreateInfo.systemId = systemId;
    result = xrCreateSession(instance, &sessionCreateInfo, &session);
    XR_CHECK(result, "Failed to create OpenXR session");

    // 4. �A�N�V�����Z�b�g�̍쐬�ƃA�^�b�`
    XrActionSet actionSet;
    XrActionSetCreateInfo actionSetInfo = { XR_TYPE_ACTION_SET_CREATE_INFO };
    strcpy_s(actionSetInfo.actionSetName, sizeof(actionSetInfo.actionSetName), "gameplay");
    strcpy_s(actionSetInfo.localizedActionSetName, sizeof(actionSetInfo.localizedActionSetName), "Gameplay");
    result = xrCreateActionSet(instance, &actionSetInfo, &actionSet);
    XR_CHECK(result, "Failed to create action set");

    XrSessionActionSetsAttachInfo attachInfo = { XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
    attachInfo.countActionSets = 1;
    attachInfo.actionSets = &actionSet;
    result = xrAttachSessionActionSets(session, &attachInfo);
    XR_CHECK(result, "Failed to attach action sets to session");

    // 5. �r���[�̐ݒ�Ɗm�F
    // �r���[�̐����擾
    uint32_t viewCount;
    result = xrEnumerateViewConfigurations(instance, systemId, 0, &viewCount, nullptr);
    XR_CHECK(result, "Failed to enumerate view configurations");

    // �r���[�̏����i�[���邽�߂̃x�N�^���쐬
    std::vector<XrView> views(viewCount, { XR_TYPE_VIEW });

    // �r���[�̐ݒ�
    XrViewLocateInfo viewLocateInfo = { XR_TYPE_VIEW_LOCATE_INFO };
    viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    viewLocateInfo.displayTime = 0; // �K�؂ȕ\�����Ԃ�ݒ�
    viewLocateInfo.space = XR_NULL_HANDLE; // �K�؂ȃX�y�[�X��ݒ�

    XrViewState viewState = { XR_TYPE_VIEW_STATE }; // XrViewState �ϐ���ǉ�
    uint32_t viewCountOutput;
    result = xrLocateViews(session, &viewLocateInfo, &viewState, views.size(), &viewCountOutput, views.data());
    XR_CHECK(result, "Failed to locate views");

    std::cout << "OpenXR is usable: Connected to HMD, input is active, and rendering is possible." << std::endl;

    // ���\�[�X�̃N���[���A�b�v
    xrDestroySession(session);
    xrDestroyInstance(instance);

    return true;
}




#include <iostream>
#include <openxr/openxr.h>
#include <cstdlib>

#define XR_CHECK(result, msg) \
    if (XR_FAILED(result)) { \
        std::cerr << msg << ": " << result << std::endl; \
        return false; \
    }

bool CheckOpenXRSystem() {
    XrInstance instance = XR_NULL_HANDLE;
    XrSystemId systemId;
    XrResult result;

    // ���ϐ��̊m�F
    char* runtimeJson = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&runtimeJson, &len, "XR_RUNTIME_JSON");
    if (err || runtimeJson == nullptr) {
        std::cerr << "XR_RUNTIME_JSON environment variable is not set or cannot be retrieved" << std::endl;
        return false;
    }
    std::cout << "XR_RUNTIME_JSON: " << runtimeJson << std::endl;
    free(runtimeJson);

    // OpenXR �C���X�^���X�̍쐬
    XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
    strncpy_s(createInfo.applicationInfo.applicationName, "MyVRApp", XR_MAX_APPLICATION_NAME_SIZE);
    createInfo.applicationInfo.applicationVersion = 1;
    strncpy_s(createInfo.applicationInfo.engineName, "CustomEngine", XR_MAX_ENGINE_NAME_SIZE);
    createInfo.applicationInfo.engineVersion = 1;
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    result = xrCreateInstance(&createInfo, &instance);
    XR_CHECK(result, "Failed to create OpenXR instance");

    // �C���X�^���X�v���p�e�B�̎擾
    XrInstanceProperties instProps = { XR_TYPE_INSTANCE_PROPERTIES };
    result = xrGetInstanceProperties(instance, &instProps);
    XR_CHECK(result, "Failed to get OpenXR instance properties");

    std::cout << "Runtime: " << instProps.runtimeName << std::endl;
    std::cout << "Version: " << XR_VERSION_MAJOR(instProps.runtimeVersion) << "."
        << XR_VERSION_MINOR(instProps.runtimeVersion) << "."
        << XR_VERSION_PATCH(instProps.runtimeVersion) << std::endl;

    // �V�X�e�����̎擾�O�ɏڍׂ��o��
    XrSystemGetInfo systemInfo = { XR_TYPE_SYSTEM_GET_INFO };
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    std::cout << "System Info: formFactor = " << systemInfo.formFactor << std::endl;

    result = xrGetSystem(instance, &systemInfo, &systemId);
    if (XR_FAILED(result)) {
        switch (result) {
        case XR_ERROR_FORM_FACTOR_UNSUPPORTED:
            std::cerr << "Failed to get OpenXR system: Form factor unsupported" << std::endl;
            break;
        case XR_ERROR_INSTANCE_LOST:
            std::cerr << "Failed to get OpenXR system: Instance lost" << std::endl;
            break;
        case XR_ERROR_SYSTEM_INVALID:
            std::cerr << "Failed to get OpenXR system: System invalid" << std::endl;
            break;
        case XR_ERROR_RUNTIME_FAILURE:
            std::cerr << "Failed to get OpenXR system: Runtime failure" << std::endl;
            break;
        default:
            std::cerr << "Failed to get OpenXR system: Unknown error" << std::endl;
        }
        return false;
    }

    std::cout << "Successfully retrieved OpenXR system with ID: " << systemId << std::endl;

    // �C���X�^���X�̉��
    xrDestroyInstance(instance);

    return true;
}




bool CheckOpenXRVersion() {
    XrInstance instance;
    XrResult result;

    XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
    strncpy_s(createInfo.applicationInfo.applicationName, "MyVRApp", XR_MAX_APPLICATION_NAME_SIZE);
    createInfo.applicationInfo.applicationVersion = 1;
    strncpy_s(createInfo.applicationInfo.engineName, "CustomEngine", XR_MAX_ENGINE_NAME_SIZE);
    createInfo.applicationInfo.engineVersion = 1;
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    result = xrCreateInstance(&createInfo, &instance);
    if (XR_FAILED(result)) {
        std::cerr << "Failed to create OpenXR instance: " << result << std::endl;
        return false;
    }

    XrInstanceProperties instProps = { XR_TYPE_INSTANCE_PROPERTIES };
    result = xrGetInstanceProperties(instance, &instProps);
    if (XR_FAILED(result)) {
        std::cerr << "Failed to get OpenXR instance properties: " << result << std::endl;
        return false;
    }

    std::cout << "OpenXR Runtime Name: " << instProps.runtimeName << std::endl;
    std::cout << "OpenXR Runtime Version: " << XR_VERSION_MAJOR(instProps.runtimeVersion) << "."
        << XR_VERSION_MINOR(instProps.runtimeVersion) << "."
        << XR_VERSION_PATCH(instProps.runtimeVersion) << std::endl;

    return true;
}



bool CheckAppContainer() {
    BOOL inAppContainer = FALSE;
    HANDLE processToken = nullptr;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &processToken)) {
        std::cerr << "Failed to open process token" << std::endl;
        return false;
    }

    DWORD returnLength = 0;
    GetTokenInformation(processToken, TokenIsAppContainer, &inAppContainer, sizeof(inAppContainer), &returnLength);

    CloseHandle(processToken);

    if (inAppContainer) {
        std::cerr << "Running in AppContainer, which may cause issues with OpenXR" << std::endl;
        return false;
    }
    else {
        std::cout << "Not running in AppContainer" << std::endl;
        return true;
    }
}


bool CheckRuntimeEnvironmentVariable() {
    char* runtimeJson = nullptr;
    size_t len = 0;

    // _dupenv_s���g�p���Ċ��ϐ����擾
    errno_t err = _dupenv_s(&runtimeJson, &len, "XR_RUNTIME_JSON");
    if (err || runtimeJson == nullptr) {
        std::cerr << "XR_RUNTIME_JSON environment variable is not set or cannot be retrieved" << std::endl;
        return false;
    }

    std::cout << "XR_RUNTIME_JSON: " << runtimeJson << std::endl;
    free(runtimeJson);  // �����������

    return true;
}

