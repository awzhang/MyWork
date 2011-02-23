#ifndef _YX_TTS_H_
#define _YX_TTS_H_ 

/* constant for TTS heap size */
//#define ivTTS_HEAP_SIZE		50000 /* 混合无音效 */
#define ivTTS_HEAP_SIZE		70000 /* 混合有音效 */

#define RESOUSE_SIZE 3459714

int InitTTS();
int ReleaseTTS();
void MakeTTS(char* v_pText);
void *PlayTTS(void *arg);
int StartTTS();
int StopTTS();

int TTSTest(char* v_pText);

#endif
