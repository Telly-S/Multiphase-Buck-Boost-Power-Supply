#include "usmart.h"
#include "usmart_str.h"
////////////////////////////�û�������///////////////////////////////////////////////
// ������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����)
#include "pwm_pid.h"
extern void v_out_target_amend(uint8_t out);
// �������б��ʼ��(�û��Լ����)
// �û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[] =
    {
#if USMART_USE_WRFUNS == 1 // ���ʹ���˶�д����
        (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
        (void *)write_addr, "void write_addr(uint32_t addr,uint32_t val)",
#endif
        (void *)pid_amend_u, "void pid_amend_u(uint16_t p, uint16_t i, uint16_t d, uint8_t mode)",
        (void *)v_out_target_amend, "void v_out_target_amend(uint8_t out)",
        // (void*)delay_ms,"void delay_ms(uint16_t nms)",
        // (void*)delay_us,"void delay_us(uint32_t nus)",
};
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// �������ƹ�������ʼ��
// �õ������ܿغ���������
// �õ�����������
struct _m_usmart_dev usmart_dev =
    {
        usmart_nametab,
        usmart_init,
        usmart_cmd_rec,
        usmart_exe,
        usmart_scan,
        sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), // ��������
        0,                                                         // ��������
        0,                                                         // ����ID
        1,                                                         // ������ʾ����,0,10����;1,16����
        0,                                                         // ��������.bitx:,0,����;1,�ַ���
        0,                                                         // ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
        0,                                                         // �����Ĳ���,��ҪPARM_LEN��0��ʼ��
};
