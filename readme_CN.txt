���ߧ����ܧ�ڧ� ��� ���էԧ���ӧܧ� ���֧է� �ܧ�ާ�ڧݧ��ڧ� �էݧ� ARM �����֧������ �ߧѧ��է���� �ߧڧا�.

ATMEGA/ATXMEGA:

���ݧ� �ܧ�ާ�ڧݧ��ڧ� �ڧ���ݧ�٧�֧���

AVR Studio 4.19 (build 730):
http://www.atmel.com/Images/AvrStudio4Setup.exe

����ݧاߧ� �ҧ��� ����ѧߧ�ӧݧ֧ߧߧ� 
Atmel AVR 8-bit and 32-bit Toolchain 3.4.2 - Windows 
http://www.atmel.com/images/avr-toolchain-installer-3.4.2.1573-win32.win32.x86.exe

ARM:

���ݧ� �ܧ�ާ�ڧݧ��ڧ� �ڧ���ݧ�٧�֧��� �ܧ�ާ�ݧ֧ܧ� �ڧߧ����ާ֧ߧ��� �� ��ѧۧ�� www.yagarto.de
�����ާ� ���ԧ�, ���� �ܧ�ާ�ڧݧ��ڧ� �էݧ� ������֧������ CORTEX-M3 �ڧ���ݧ�٧�֧��� �ҧڧҧݧڧ��֧ܧ� CMSIS ��� ARM LIMITED. ���ѧ�ѧݧ�� �ߧѧ��էڧ��� �ߧ� ���� �ا� ����ӧߧ�, ���� �� �ܧѧ�ѧݧ�� ����֧ܧ��. 

�����ѧߧ�ӧڧ�� �էӧ� �ڧߧ��ѧݧݧ��ܧ� �C 
yagarto-bu-2.23.1_gcc-4.7.2-c-c++_nl-1.20.0_gdb-7.5.1_eabi_20121222.exe
yagarto-tools-20121018-setup.exe

���ҧߧ�ӧݧ֧ߧڧ� �ܧ�ާ�ڧݧ����� - ��֧�֧�� ����: https://launchpad.net/gcc-arm-embedded
�����ݧ֧էߧ�� �ӧ֧��ڧ� - 
https://developer.arm.com/-/media/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-win32.exe
See:
https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
Old product page:
https://launchpad.net/gcc-arm-embedded

CMSIS:
https://silver.arm.com/download/ARM_and_AMBA_Architecture/CMSIS-SP-00300-r4p4-00rel0/CMSIS-SP-00300-r4p4-00rel0.tgz (.zip - format file inside)
See:
https://developer.arm.com/embedded/cmsis
https://github.com/ARM-software/CMSIS_5/releases/tag/5.2.0

���ҧ�֧�:

����ҧ�� ��֧ݧ֧ӧ�� �ܧ�ߧ�ڧԧ��ѧ�ڧ� ����֧ܧ�� ����ڧ٧ӧ�էڧ��� �� ��ѧۧݧ� product.h
���� ���ߧ�ӧѧߧڧ� �ӧ�ҧ�ѧߧ�� �ܧ�ߧ�ڧԧ��ѧ�ڧ� �� ��֧ݧ֧ӧ�ԧ� �����֧����� (arm/atmega) �ӧ�ҧڧ�ѧ֧��� ��ѧ�� �ܧ�ߧ�ڧԧ��ѧ�ڧ�ߧߧ�� ��ѧۧݧ��
.\board\*ctlstyle*.h �� .\board\*cpustyle*.h.
ctlstyle ���ڧ��ӧѧ�� �ӧߧ֧�ߧڧ� (��� ���ߧ��֧ߧڧ� �� �����֧�����) ����ҧ֧ߧߧ���� �ѧ��ѧ�ѧ���� - �ѧէ�֧�� ������ۧ��� �ߧ� ��ڧߧ� spi, 
��ڧ�� ���ڧާ֧ߧקߧߧ�� �ާڧܧ����֧� �� ��ڧ� �ڧߧէڧܧѧ���� (�� ��ѧ� �էѧݧ֧�). 
cpustyle ���ڧ��ӧѧ�� �ߧѧ٧ߧѧ�֧ߧڧ� �ӧ�ӧ�է�� �����֧����� (��ѧ���֧է֧ݧ֧ߧڧ� ��� �����ѧ� �ӧӧ�է�/�ӧ�ӧ�է�).


