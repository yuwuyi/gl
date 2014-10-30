#ifndef _IMGUI_STUFF_H_
#define _IMGUI_STUFF_H_

//#define FIXED
struct ImDrawList;
void InitImGui();
void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count);
void UpdateImGui();
void UnLoadDrawingData();

#endif