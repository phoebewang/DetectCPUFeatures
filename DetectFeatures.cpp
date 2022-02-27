#include <cpuid.h>
#include <string>
#include <sstream>
#include <iostream>
#include <regex>

using namespace std;

asm(".data\n\"?rawCPUID@@3PADA\":\nrawCPUID: .incbin \"cpuid.h\"");
extern char rawCPUID[];

int main() {
  auto SS = stringstream(rawCPUID);
  unsigned int EAX, EBX, ECX, EDX, *pREG;
  unsigned int Leaf, SubLeaf = 0;
  regex RFeature("#define bit_(\\w+)\\s+(\\d[0-9a-fx]*)");
  smatch Result;
  for (string L; getline(SS, L, '\n');) {
    if (L.substr(0, 12) == "/* Features ") {
      string R = L.substr(15, 5);
      if (R == "\%eax ") {
        pREG = &EAX;
      } else if (R == "\%ebx ") {
        pREG = &EBX;
      } else if (R == "\%ecx ") {
        pREG = &ECX;
      } else if (R == "\%edx ") {
        pREG = &EDX;
      } else {
        cout << "Error parsing REG in " << L;
        break;
      }
      regex RLeaf("leaf ([0-9a-fx]+)");
      if (!regex_search(L, Result, RLeaf)) {
        cout << "Error parsing Leaf in " << L;
        break;
      }
      Leaf = stoul(Result[1].str(), nullptr, 0);
      regex RSubLeaf("sub-leaf (\\d+)");
      SubLeaf = regex_search(L, Result, RSubLeaf) ? stoul(Result[1].str()) : 0;
      if (!__get_cpuid_count(Leaf, SubLeaf, &EAX, &EBX, &ECX, &EDX)) {
        cout << "Error parsing sub-leaf in " << L;
        break;
      }
    } else if (regex_search(L, Result, RFeature)) {
      if (*pREG & stoul(Result[2].str(), nullptr, 0))
        cout << Result[1].str() << ", ";
    }
  }
  cout << endl;
  return 0;
}
