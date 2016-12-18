#include <conio.h>
#include <io.h>
#include <fcntl.h>

char scr[4000];

int hexdigit(char c) {
  if(c>='0' && c<='9') return c-'0';
  if(c>='A' && c<='F') return c-'A'+10;
  if(c>='a' && c<='f') return c-'a'+10;
  return -1;
}

void charstr(char c,int n) {
  int i;

  for(i=0;i<n;++i) putch(c);
}

void putstr(int y,char *s) {
  int i,l;

  for(i=l=0;s[i];++i) if(s[i]=='@') switch(s[++i]) {
    case 'b':
    case 't': ++i;break;
    case '@': ++l;break;
    case 0: --i;break;
  }else ++l;
  gotoxy((66-l)/2+1,y);
  for(i=0;s[i];++i) if(s[i]=='@') switch(s[++i]) {
    case 't': textcolor(hexdigit(s[++i]));break;
    case 'b': textbackground(hexdigit(s[++i]));break;
    case '@': putch('@');break;
    case 0: --i;break;
  }else putch(s[i]);
}

void make_scr(void) {
  textmode(C80);textattr(7);clrscr();
  window(8,2,73,23);textattr(0x4E);clrscr();
  window(8,2,74,23);textattr(4);
  gotoxy(1,1);charstr('�',66);
//  gotoxy(1,22);charstr('�',66);
  textattr(0x4E);
  gotoxy(2,3);charstr('�',64);
  putstr(2,"@tfDoom2D@te");
  putstr(4,"ᤥ���� @tbPrikol Software@te � 1996 �� ���� @tfDOOM'� II");
  putstr(5,"@te� ��த� ����⥥��� ��᪮�᪮� ������");
  putstr(7,"@te�� ��ࠫ� � �������� ����� @tfDoom'� 2D@te. � ��� ⮫쪮");
  putstr(8,"���� �஢���, � deathmatch ����� ����� �� ����� 10 �����,");
  putstr(9,"����� ��������� ᢮� WAD'� (�஢�� � ��䨪�).");
  putstr(11,"� ������ ���ᨨ ��� ���� ����� 30 �஢���, ���� �������,");
  putstr(12,"����� ��㦨� � ����� �ਪ����. �� ᬮ��� ᮧ������ ᢮�");
  putstr(13,"�஢�� � ����� � deathmatch ᪮�쪮 㣮���.");
  putstr(15,"������ ����� ����� � ��᪢� � ��砫� ������ 1996 ����.");
  putstr(16,"��������, �� ᬮ��� ���� �� �� ��⨭᪮� �뭪�.");
  putstr(18,"@tb�ணࠬ��@te: ����ᥩ ����᪮�");
  putstr(19,"@tb��䨪�@te: ��ਠ� ��ଠ�, ����� ����, ������� ����㭮�");
  putstr(20,"@tb���@te: ���� ������, ������ �ਭ�");
  putstr(21,"@tb��몠@te: �������� �������");
  window(1,1,80,25);gotoxy(1,24);
  if(!getch()) getch();
}

int main() {
  int h;

  make_scr();
  window(1,1,80,25);gotoxy(1,25);
  gettext(1,1,80,25,scr);
  h=_creat("endoom.lmp",0x20);
  write(h,scr,4000);
  close(h);
  return 0;
}
