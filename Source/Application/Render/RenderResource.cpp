#include "RenderResource.h"


namespace Matrix
{
	void RenderResource::resetRingBufferOffset(uint8_t current_frame_index)
	{
		//����buffer�� begin�����ʼ������end��ǻ�������������size��¼����ռ� end = begin �ͷ��Ѿ�ʹ�õ�buffer ring��
		m_global_render_resource._storage_buffer._global_upload_ringbuffers_end[current_frame_index] =
			m_global_render_resource._storage_buffer._global_upload_ringbuffers_begin[current_frame_index];
	}
}


