#ifndef LEFTIST_HEAP_HPP
#define LEFTIST_HEAP_HPP //C++ Header leftist_heap.hpp

#include<algorithm>
#include<memory>

namespace oitl
{

#if __cplusplus<201103L
	#ifdef nullptr
		#undef nullptr
	#endif
	#define nullptr NULL
#endif

/************************************************
 * This class hasn't been finished
 * modify() is unavailable
 * Firstly it's O(n), almost useless
 * Secondly what I write is wrong so I delete it
 * 
 * TODO: Finish modify()
************************************************/

template<typename _Tp,typename _Cmp=std::less<_Tp>,typename _Alloc=std::allocator<_Tp> >
class leftist_heap:_Cmp
{
	private:
        struct Node;
        Node* _root;
        size_t s;
        Node* merge(Node*,Node*);

		typedef typename _Alloc::template rebind<Node>::other _node_alloc_type;
	#if __cplusplus>=201103L
		typedef typename std::allocator_traits<_Alloc>::template rebind_traits<Node> _node_alloc_traits_type;
	#endif

		_node_alloc_type _node_allocator;
		inline Node *__get_new_node(_Tp);
		inline void __delete_node(Node*);

    public:
		typedef _Tp value_type;
		typedef _Cmp cmp_type;
		typedef size_t size_type;
		typedef _Alloc allocator_type;
	#if __cplusplus>=201103L
		typedef std::allocator_traits<_Alloc> _alloc_traits_type;
	#endif

        struct iterator;
        iterator push(const _Tp&);
        value_type &top()const;
        iterator pop();
        iterator join(leftist_heap&);
		bool modify(const iterator&,const value_type&);
        bool empty()const;
        size_type size()const;
};

template<typename _Tp,typename _Cmp,typename _Alloc>
struct leftist_heap<_Tp,_Cmp,_Alloc>::Node
{
    _Tp val;
    Node *ftr,*lc,*rc;
    size_t npl;
    Node(_Tp Value):
        val(Value),
		ftr(nullptr),
        lc(nullptr),
        rc(nullptr),
        npl(0)
	{}

	void maintain()
	{
		size_t ldp=0,rdp=0;
		if(lc!=nullptr)
		{
			ldp=lc->npl+1;
			lc->ftr=this;
		}
		if(rc!=nullptr)
		{
			rdp=rc->npl+1;
			rc->ftr=this;
		}
		npl=std::min(ldp,rdp);
		if(ldp<rdp)std::swap(lc,rc);
	}
};

template<typename _Tp,typename _Cmp,typename _Alloc>
inline
typename leftist_heap<_Tp,_Cmp,_Alloc>::Node*
leftist_heap<_Tp,_Cmp,_Alloc>::__get_new_node(_Tp __value)
{
#if __cplusplus>=201103L
    Node *new_ptr=_node_alloc_traits_type::allocate(_node_allocator,1);
    _node_alloc_traits_type::construct(_node_allocator,new_ptr,__value);
#else
	Node *new_ptr=_node_allocator.allocate(1);
	_node_allocator.construct(new_ptr,__value);
#endif
    return new_ptr;
}

template<typename _Tp,typename _Cmp,typename _Alloc>
inline
void
leftist_heap<_Tp,_Cmp,_Alloc>::__delete_node(Node *__ptr)
{
#if __cplusplus>=201103L
    _node_alloc_traits_type::destroy(_node_allocator,__ptr);
    _node_alloc_traits_type::deallocate(_node_allocator,__ptr,1);
#else
	_node_allocator.destroy(__ptr);
	_node_allocator.deallocate(__ptr,1);
#endif
}

template<typename _Tp,typename _Cmp,typename _Alloc>
typename
leftist_heap<_Tp,_Cmp,_Alloc>::Node*
leftist_heap<_Tp,_Cmp,_Alloc>::merge(Node* first_heap,Node* second_heap)
{
    if(first_heap==NULL)return second_heap;
    if(second_heap==NULL)return first_heap;
    if(_Cmp::operator()(second_heap->val,first_heap->val))
	{
		std::swap(first_heap,second_heap);
	}
    second_heap->rc=merge(first_heap,second_heap->rc);
    if(second_heap->lc==NULL||second_heap->lc->npl<second_heap->rc->npl)
	{
		std::swap(second_heap->lc,second_heap->rc);
	}
    second_heap->maintain();
    return second_heap;
}

template<typename _Tp,typename _Cmp,typename _Alloc>
struct leftist_heap<_Tp,_Cmp,_Alloc>::iterator
{
    private:
        Node *real_node;
		friend class leftist_heap;
        iterator(Node* ptr):real_node(ptr) {}
    public:
		iterator():real_node(nullptr){}

        const _Tp &operator*()const
        {
            return real_node->val;
        }
        operator bool()const
		{
			return real_node!=nullptr;
		}
        bool operator==(const iterator& rhs)const
		{
			return real_node==rhs.__real_node;
		}
        bool operator!=(const iterator& rhs)const
		{
			return real_node!=rhs.real_node;
		}
};

template<typename _Tp,typename _Cmp,typename _Alloc>
typename
leftist_heap<_Tp,_Cmp,_Alloc>::iterator
leftist_heap<_Tp,_Cmp,_Alloc>::push(const _Tp& Value)
{
    Node* new_node=new Node(Value);
    _root=merge(new_node,_root);
    ++s;
    return iterator(new_node);
}

template<typename _Tp,typename _Cmp,typename _Alloc>
typename
leftist_heap<_Tp,_Cmp,_Alloc>::value_type&
leftist_heap<_Tp,_Cmp,_Alloc>::top()const
{
    return _root->val;
}

template<typename _Tp,typename _Cmp,typename _Alloc>
typename
leftist_heap<_Tp,_Cmp,_Alloc>::iterator
leftist_heap<_Tp,_Cmp,_Alloc>::pop()
{
    Node* ptr=_root;
	if(ptr->lc!=nullptr)ptr->lc->ftr=nullptr;
	if(ptr->rc!=nullptr)ptr->rc->ftr=nullptr;
    _root=merge(ptr->lc,ptr->rc);
    __delete_node(ptr);
    --s;
    return iterator(_root);
}

template<typename _Tp,typename _Cmp,typename _Alloc>
typename
leftist_heap<_Tp,_Cmp,_Alloc>::iterator
leftist_heap<_Tp,_Cmp,_Alloc>::join(leftist_heap<_Tp,_Cmp,_Alloc>& Other_heap)
{
    _root=merge(Other_heap._root,_root);
    s+=Other_heap.s;
    Other_heap._root=NULL;
    Other_heap.s=0;
    return iterator(_root);
}

template<typename _Tp,typename _Cmp,typename _Alloc>
typename
leftist_heap<_Tp,_Cmp,_Alloc>::size_type
leftist_heap<_Tp,_Cmp,_Alloc>::size()const
{
    return s;
}

template<typename _Tp,typename _Cmp,typename _Alloc>
bool
leftist_heap<_Tp,_Cmp,_Alloc>::empty()const
{
    return _root==NULL;
}

} //namespace oitl

#endif