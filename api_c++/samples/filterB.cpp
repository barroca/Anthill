
#include "filterB.h"


int filterB::init(){

	cout << "init " << get_filter_name() << " Id: " 
		<< get_global_filter_thread_id() << ". Rank: " << get_my_rank() << "\n";

	if(has_out_port("out")) {
		get_out_port_names();
	}
	if(has_in_port("in")) {
		get_in_port_names();
	}

	set_param("lama_key", "lama_val");
	set_param("test_key", "test_val");

	if(has_param("test_key")) cout << "Test: " << get_param("test_key") << "\n";
	if(has_param("lama_key")) cout << "Lama: " << get_param("lama_key") << "\n";

	return 0;
}
int filterB::process(){

	cout << "process " << get_filter_name() << " Id: " 
		<< get_global_filter_thread_id() << ". Rank: " << get_my_rank() << "\n";

	if(any_upstream_running()) {
		DCBuffer * inBuffer = read ("in");
		std::string inStr;
		inBuffer->unpack("s", &inStr);
		cout << "Buffer : " << inStr << "\n";
		inBuffer->consume();
	}

	if(any_upstream_running()) {
		DCBuffer * inBuffer1;
		std::string inStr;
		do {
			inBuffer1 = read_non_blocking ("in");
		}while (inBuffer1 == NULL);
		inBuffer1->unpack("s", &inStr);
		cout << "Buffer : " << inStr << "\n";
	}

	return 0;
}

int filterB::fini(){
	cout << "finalize " << get_filter_name() << " Id: " 
		<< get_global_filter_thread_id() << " . Rank: " << get_my_rank() << "\n";
	return 0;
}

provide1(filterB)

/*void * get_filter() { 
	return new filterB(); 
}*/


/*int main () {
        printf ("%x\n",get_filter());
	//filterB *filter = new filterB();

	return 0;
}*/
