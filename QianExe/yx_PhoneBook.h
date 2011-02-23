#if !defined(PHOTOEBOOK_H_Q94FKGSL4378)
#define PHOTOEBOOK_H_Q94FKGSL4378

#if USE_TELLIMIT == 1

class CPhoneBook
{
private:
	BookCfg m_cfg;
		
public:
	void hdl_cent(byte datype, char *dat, ulong len);
	void tcp_send(byte trantype, byte datatype, void *data, int len);
	void hdl_3301(byte *dat, ulong len);
	void hdl_3302(byte *dat, ulong len);	
};

#endif

#endif

