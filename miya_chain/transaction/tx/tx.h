#ifndef E0075B66_D135_42BE_AFC2_8CD23A5B3DB4
#define E0075B66_D135_42BE_AFC2_8CD23A5B3DB4



namespace tx
{






template <typename T>
class Tx // shared_ptrで管理したい為
{
private:
	T* _ptr;

public:
	void set(T *target ); // setter
	T* get(); // getter

};






/*
 - トランザクションの検証について

*/





}; // close namespace tx



#endif // 


