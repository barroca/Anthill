
#include "filterA.h"


int filterA::init(){

	cout << "init " << get_filter_name() << " Id: " 
		<< get_global_filter_thread_id() << ". Rank: " << get_my_rank() << "\n";

	if(has_out_port("out")) {
		get_out_port_names();
	}
	if(has_in_port("in")) {
		get_in_port_names();
	}

	return 0;
}
int filterA::process(){

	cout << "process " << get_filter_name() << " Id: " 
		<< get_global_filter_thread_id() << ". Rank: " << get_my_rank() << "\n";

	DCBuffer * outBuffer = new DCBuffer(100);
	outBuffer->pack("s", "Hellow!!!");

	write (outBuffer, "out");
	outBuffer->Delete();

	DCBuffer * outBuffer1 = new DCBuffer(100);
	outBuffer1->pack("s", "NonBlocking message.");

	write_nocopy(outBuffer1, "out");
	outBuffer1->Delete();	

	//filter_exit("Nao e' nada nao");

	return 0;
}

int filterA::fini(){
	cout << "finalize " << get_filter_name() <<  " Id: " 
		<< get_global_filter_thread_id() << ". Rank: " << get_my_rank() << "\n";
	return 0;
}

provide1(filterA)

/*void * get_filter() { 
	return new filterA(); 
}*/


/*int main () {
        printf ("%x\n",get_filter());
	//filterA *filter = new filterA();

	return 0;
}*/
