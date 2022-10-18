#include "RenderResource.h"


namespace Matrix
{
	void RenderResource::resetRingBufferOffset(uint8_t current_frame_index)
	{
		//环形buffer， begin标记起始索引，end标记活动区域结束索引，size记录最大活动空间 end = begin 释放已经使用的buffer ring。
		m_global_render_resource._storage_buffer._global_upload_ringbuffers_end[current_frame_index] =
			m_global_render_resource._storage_buffer._global_upload_ringbuffers_begin[current_frame_index];
	}
}


