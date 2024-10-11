#include "miniaudio.h"
#include "rlImGui/rlImGui.h"
#include <imgui.h>
#include <iostream>
#include <raylib.h>
#include <string>

void list_audio_devices() {
  ma_result result;
  ma_context context;
  ma_device_info *pPlaybackDeviceInfos;
  ma_uint32 playbackDeviceCount;
  ma_device_info *pCaptureDeviceInfos;
  ma_uint32 captureDeviceCount;
  ma_uint32 iDevice;

  if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
    std::cout << "failed to initialize context" << std::endl;
  }

  if (ma_context_get_devices(&context, &pPlaybackDeviceInfos,
                             &playbackDeviceCount, &pCaptureDeviceInfos,
                             &captureDeviceCount) != MA_SUCCESS) {
    std::cout << "failed to retrieve device information" << std::endl;
  }

  std::cout << "playback devices:" << std::endl;
  for (int i = 0; i < playbackDeviceCount; ++i) {
    std::cout << pPlaybackDeviceInfos[i].name << std::endl;
  }
  std::cout << std::endl;

  std::cout << "capture devices:" << std::endl;
  for (int i = 0; i < captureDeviceCount; ++i) {
    std::cout << pCaptureDeviceInfos[i].name << std::endl;
  }

  ma_context_uninit(&context);
}

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                   ma_uint32 frameCount) {
  // MA_ASSERT(pDevice->capture.format == pDevice->playback.format);
  // MA_ASSERT(pDevice->capture.channels == pDevice->playback.channels);

  std::cout << frameCount << std::endl;

  memcpy(pOutput, pInput,
         frameCount * ma_get_bytes_per_frame(pDevice->capture.format,
                                             pDevice->capture.channels));
}

int main(int argc, char *args[]) {
  std::cout << "main" << std::endl;
  ma_device_config deviceConfig;
  ma_device device;
  ma_context context;
  ma_device_info *pPlaybackDeviceInfos;
  ma_uint32 playbackDeviceCount;
  ma_device_info *pCaptureDeviceInfos;
  ma_uint32 captureDeviceCount;
  ma_uint32 iDevice;

  if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
    std::cout << "failed to initialize context" << std::endl;
  }

  if (ma_context_get_devices(&context, &pPlaybackDeviceInfos,
                             &playbackDeviceCount, &pCaptureDeviceInfos,
                             &captureDeviceCount) != MA_SUCCESS) {
    std::cout << "failed to retrieve device information" << std::endl;
  }

  std::cout << "playback devices:" << std::endl;
  for (int i = 0; i < playbackDeviceCount; ++i) {
    std::cout << pPlaybackDeviceInfos[i].name << std::endl;
  }
  std::cout << std::endl;

  std::cout << "capture devices:" << std::endl;
  for (int i = 0; i < captureDeviceCount; ++i) {
    std::cout << pCaptureDeviceInfos[i].name << std::endl;
  }

  deviceConfig = ma_device_config_init(ma_device_type_duplex);
  deviceConfig.capture.pDeviceID = &pCaptureDeviceInfos[2].id;
  deviceConfig.capture.format = ma_format_s16;
  deviceConfig.capture.channels = 2;
  deviceConfig.capture.shareMode = ma_share_mode_shared;
  deviceConfig.playback.pDeviceID = &pPlaybackDeviceInfos[1].id;
  deviceConfig.playback.format = ma_format_s16;
  deviceConfig.playback.channels = 2;
  deviceConfig.dataCallback = data_callback;

  if (ma_device_init(&context, &deviceConfig, &device) != MA_SUCCESS) {
    std::cout << "failed to init device" << std::endl;
  }

  ma_device_start(&device);
  getchar();

  ma_device_uninit(&device);

  // const int screenWidth = 1280;
  // const int screenHeight = 720;
  //
  // InitWindow(screenWidth, screenHeight, "raylibapp");
  // SetTargetFPS(60);
  // rlImGuiSetup(true);
  //
  // while (!WindowShouldClose()) {
  //   BeginDrawing();
  //   ClearBackground(RAYWHITE);
  //
  //   DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);
  //
  //   rlImGuiBegin();
  //   ImGui::Button("click me");
  //   bool open = true;
  //   ImGui::ShowDemoWindow(&open);
  //   rlImGuiEnd();
  //
  //   EndDrawing();
  // }
  //
  // rlImGuiShutdown();
  // CloseWindow();
}
