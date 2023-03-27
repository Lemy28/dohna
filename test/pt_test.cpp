#include <tools/ProcessTitle.h>
#include <unistd.h>


int main(int argc,char**argv){
    ProcessTitle pt(argc,argv,environ);
    pt.setProcTitle("dohna:test");

    
    sleep(999);
}