//
// Created by weiya.gao on 12/25/2023.
//

#pragma once
#ifdef HAI_EXPORT
#   ifdef  _WIN32
#       define HAI_CALL __declspec(dllexport)
#   else
#       define HAI_CALL __attribute__ ((visibility ("default")))
#   endif
#else
#   define HAI_CALL
#endif
#ifdef __cplusplus
extern "C" {
#endif
HAI_CALL void *
hvid_record_open(int work_gpu, int v_width, int v_height, int framerate, int level, const char *export_dir);
HAI_CALL int hvid_record_get_vid_frame_buffsize(void *inst_id);
HAI_CALL int hvid_record_get_vid_frame_count(void *inst_id);
HAI_CALL int hvid_record_write_vid(void *inst_id, const char *vid_buff, int vid_buff_size, int is_final);
HAI_CALL int hvid_record_write_aud(void *inst_id, const char *aud_buff, int aud_buff_size, bool is_final);
HAI_CALL int hvid_record_close(void *inst_id);
#ifdef __cplusplus
}
#endif
