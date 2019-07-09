Dzień dobry, wydaje mi się że alokacja działa bardzo dobrze, ale mam jeden problem (udostępniony screen)

Multiboot mówi o dwóch dostępnych obszarach - AVAILABLE
Z tym pierwszym obszarem, w pamięci niskiej, nie ma problemu i mogę nawet wyzerować cały ten obszar
Z tym drugim obszarem w pamięci wysokiej jest jednak problem, bo gdy chce wyzerować jego bajty (Z pierwszymi 100 bajtami akurat działa, problem się pojawia gdy chce wyzerować ponad 800)
to wtedy albo przestają działać przerwania, przerwania są przypadkowe, albo QUEMU po prostu się wyłącza.

Czy w tym obszarze, pomimo tego że jest AVAILABLE mogą być jakieś istotne dane? Jakie? I co z tym zrobić?

