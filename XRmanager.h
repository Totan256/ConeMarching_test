#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <vector>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

// エラーチェック用マクロ
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

    // 1. OpenXR インスタンスの作成
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

    // 2. システムの取得
    XrSystemGetInfo systemInfo = { XR_TYPE_SYSTEM_GET_INFO };
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    result = xrGetSystem(instance, &systemInfo, &systemId);
    XR_CHECK(result, "Failed to get OpenXR system or HMD not found");

    // 3. セッションの作成
    XrSessionCreateInfo sessionCreateInfo = { XR_TYPE_SESSION_CREATE_INFO };
    sessionCreateInfo.systemId = systemId;
    result = xrCreateSession(instance, &sessionCreateInfo, &session);
    XR_CHECK(result, "Failed to create OpenXR session");

    // 4. アクションセットの作成とアタッチ
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

    // 5. ビューの設定と確認
    // ビューの数を取得
    uint32_t viewCount;
    result = xrEnumerateViewConfigurations(instance, systemId, 0, &viewCount, nullptr);
    XR_CHECK(result, "Failed to enumerate view configurations");

    // ビューの情報を格納するためのベクタを作成
    std::vector<XrView> views(viewCount, { XR_TYPE_VIEW });

    // ビューの設定
    XrViewLocateInfo viewLocateInfo = { XR_TYPE_VIEW_LOCATE_INFO };
    viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    viewLocateInfo.displayTime = 0; // 適切な表示時間を設定
    viewLocateInfo.space = XR_NULL_HANDLE; // 適切なスペースを設定

    XrViewState viewState = { XR_TYPE_VIEW_STATE }; // XrViewState 変数を追加
    uint32_t viewCountOutput;
    result = xrLocateViews(session, &viewLocateInfo, &viewState, views.size(), &viewCountOutput, views.data());
    XR_CHECK(result, "Failed to locate views");

    std::cout << "OpenXR is usable: Connected to HMD, input is active, and rendering is possible." << std::endl;

    // リソースのクリーンアップ
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

    // 環境変数の確認
    char* runtimeJson = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&runtimeJson, &len, "XR_RUNTIME_JSON");
    if (err || runtimeJson == nullptr) {
        std::cerr << "XR_RUNTIME_JSON environment variable is not set or cannot be retrieved" << std::endl;
        return false;
    }
    std::cout << "XR_RUNTIME_JSON: " << runtimeJson << std::endl;
    free(runtimeJson);

    // OpenXR インスタンスの作成
    XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
    strncpy_s(createInfo.applicationInfo.applicationName, "MyVRApp", XR_MAX_APPLICATION_NAME_SIZE);
    createInfo.applicationInfo.applicationVersion = 1;
    strncpy_s(createInfo.applicationInfo.engineName, "CustomEngine", XR_MAX_ENGINE_NAME_SIZE);
    createInfo.applicationInfo.engineVersion = 1;
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    result = xrCreateInstance(&createInfo, &instance);
    XR_CHECK(result, "Failed to create OpenXR instance");

    // インスタンスプロパティの取得
    XrInstanceProperties instProps = { XR_TYPE_INSTANCE_PROPERTIES };
    result = xrGetInstanceProperties(instance, &instProps);
    XR_CHECK(result, "Failed to get OpenXR instance properties");

    std::cout << "Runtime: " << instProps.runtimeName << std::endl;
    std::cout << "Version: " << XR_VERSION_MAJOR(instProps.runtimeVersion) << "."
        << XR_VERSION_MINOR(instProps.runtimeVersion) << "."
        << XR_VERSION_PATCH(instProps.runtimeVersion) << std::endl;

    // システム情報の取得前に詳細を出力
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

    // インスタンスの解放
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

    // _dupenv_sを使用して環境変数を取得
    errno_t err = _dupenv_s(&runtimeJson, &len, "XR_RUNTIME_JSON");
    if (err || runtimeJson == nullptr) {
        std::cerr << "XR_RUNTIME_JSON environment variable is not set or cannot be retrieved" << std::endl;
        return false;
    }

    std::cout << "XR_RUNTIME_JSON: " << runtimeJson << std::endl;
    free(runtimeJson);  // メモリを解放

    return true;
}

