/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

#ifndef GIL_PRINT_TYPES_HPP
#define GIL_PRINT_TYPES_HPP

#ifdef NO_ASL
#include <gil/extension/dynamic_image/type_vector.hpp>
#include <gil/extension/dynamic_image/any_image_factory.hpp>
#include <gil/extension/dynamic_image/apply_operation.hpp>
#include <gil/core/color_convert.hpp>
#include <gil/core/image_view_factory.hpp>
#else
#include <adobe/gil/extension/dynamic_image/type_vector.hpp>
#include <adobe/gil/extension/dynamic_image/any_image_factory.hpp>
#include <adobe/gil/extension/dynamic_image/apply_operation.hpp>
#include <adobe/gil/core/color_convert.hpp>
#include <adobe/gil/core/image_view_factory.hpp>
#endif

//////////////////////////////////////////////////
///
/// Debugging code - prints types and type vectors
///
//////////////////////////////////////////////////
ADOBE_GIL_NAMESPACE_BEGIN

struct error_t;

template <typename T> void print(const T& t);

template <> inline void print(const error_t&) { printf("ERROR"); }
template <> inline void print(const unsigned char&) { printf("8"); }
template <> inline void print(const unsigned short&) { printf("16"); }
template <> inline void print(const float&) { printf("32"); }
template <> inline void print(const rgb_t&) { printf("rgb"); }
template <> inline void print(const bgr_t&) { printf("bgr"); }
template <> inline void print(const lab_t&) { printf("lab"); }
template <> inline void print(const hsb_t&) { printf("hsb"); }
template <> inline void print(const cmyk_t&) { printf("cmyk"); }
template <> inline void print(const rgba_t&) { printf("rgba"); }
template <> inline void print(const argb_t&) { printf("argb"); }
template <> inline void print(const bgra_t&) { printf("bgra"); }
template <> inline void print(const abgr_t&) { printf("abgr"); }
template <> inline void print(const gray_t&) { printf("gray"); }

template <> inline void print(const boost::mpl::true_&) { printf("true"); }
template <> inline void print(const boost::mpl::false_&) { printf("false"); }

template <typename T> 
void print(const T* t) {
    printf("const ");print(*t); printf("*");
}

template <typename T> 
void print(T* t) {
    print(*t); printf("*");
}

template <typename T, T N> 
void print(const typename boost::mpl::integral_c<T,N>&) { printf("%d",N); }

template <long N> 
void print(const typename boost::mpl::long_<N>&) { printf("%d",N); }
template <long N> 
void print(const typename boost::mpl::size_t<N>&) { printf("%d",N); }

template <typename T, typename Cs> 
void print(const pixel<T,Cs>&) {
    print(Cs());
    print(T());
    printf("_pixel_t");
}

template <typename L> 
void print(const image<L>&) {
    print(typename image<L>::color_space_t());
    print(typename image<L>::view_t::channel_t());
    if (pixel_iterator_traits<typename L::x_iterator>::is_planar)
        printf("_planar");
    printf("_image_t");
}

// rgb8
template <typename T, typename C>
void print_iterator_core(pixel<T,C>*) {
    print(C());
    print(T());
}

// rgb8c
template <typename T, typename C>
void print_iterator_core(const pixel<T,C>* ptr) {
    print_iterator_core((pixel<T,C>*)ptr);
    printf("c");
}

// rgb8_planar
template <typename T, typename C>
void print_iterator_core(const planar_ptr<T&,C>&) {
    pixel<T,C>* p;
    print_iterator_core(p);
    printf("_planar");
}

// rgb8c_planar
template <typename T, typename C>
void print_iterator_core(const planar_ptr<const T&,C>&) {
    const pixel<T,C>* p;
    print_iterator_core(p);
    printf("_planar");
}

// ..._step
template <typename IT>
void print_iterator_core(const pixel_step_iterator<IT>&) {
    print_iterator_core(IT());
    printf("_step");
}

// ..._to_rgb8
template <typename XIterator, typename DstP> 
void print_iterator_core(dereference_iterator_adaptor<XIterator, color_convert_deref_fn<typename pixel_iterator_traits<XIterator>::const_reference, DstP> >) {
    print_iterator_core(XIterator());
    printf("_to_");
    print(typename DstP::color_space_t());
    print(typename DstP::channel_t());
}

template <typename L> 
void print(const image_view<L>& iv) {
    print_iterator_core(typename image_view<L>::x_iterator());
    printf("_view_t");
}
/*

template <typename L, typename XIterator> 
void print(const image_view<L>&, XIterator) {
    print(typename image_view<L>::color_space_t());
    print(typename image_view<L>::channel_t());

    if (boost::is_same<image_view<L>, typename image_view<L>::const_t>::value)
        printf("c");
    if (pixel_iterator_traits<typename L::x_iterator>::is_planar)
        printf("_planar");
    if (iterator_is_step<typename L::x_iterator>::value)
        printf("_step");
}

template <typename L, typename XIterator> 
void print_iterator(const image_view<L>& v, pixel_step_iterator<XIterator>) {
    printf("_step");
    print(v, XIterator());
}

template <typename L, typename XIterator, typename DstP> 
void print(const image_view<L>&, dereference_iterator_adaptor<XIterator, color_convert_deref_fn<typename pixel_iterator_traits<XIterator>::const_reference, DstP> >) {
    typedef typename pixel_iterator_traits<XIterator>::value_type orig_pix_t;
    print(typename orig_pix_t::color_space_t());
    print(typename orig_pix_t::channel_t());
    printf("_to_");
    print(typename image_view<L>::color_space_t());
    print(typename image_view<L>::channel_t());
//    if (boost::is_same<image_view<L>, typename image_view<L>::const_t>::value)
//        printf("c");
    if (pixel_iterator_traits<typename L::x_iterator>::is_planar)
        printf("_planar");
    if (iterator_is_step<typename L::x_iterator>::value)
        printf("_step");
}
*/
namespace detail {
    template <typename Types, long K, long N> 
    struct print_vec_fn {
        void operator()() {
            print(typename boost::mpl::at_c<Types,K>::type());
            if (K+1<N)
                printf(" ");
            print_vec_fn<Types,K+1,N>()();
        }
    };

    template <typename Types, long N> 
    struct print_vec_fn<Types,N,N> { void operator()() {printf("]");} };
}

template <typename Types> 
void print_vec(const Types&) {
    printf("(%d) [", boost::mpl::size<Types>::value);
    detail::print_vec_fn<Types, 0, boost::mpl::size<Types>::value>()();
}

struct print_op {
    typedef void result_type;
    template <typename T>
    void operator()(const T& t) const { print(t); }
};

template <typename Types> 
void print_vec(const any_image<Types>& ai)      { 
    printf("AnyImage (%d) = ",boost::mpl::size<Types>::value);
    apply_operation(ai, print_op());
    printf("\n");
    print_vec(Types()); 
    printf("\n");
}

template <typename Types> 
void print_vec(const any_image_view<Types>& ai) { 
    printf("AnyImageView (%d) = ",boost::mpl::size<Types>::value);
    apply_operation(ai, print_op());
    printf("\n");
    print_vec(Types()); 
    printf("\n");
}

template <typename Fn> 
void print_fn(const Fn& fn) { 
    printf(" ");
}

inline void print_order_vec(const kInterleavedOnly&) { printf("Interleaved"); }
inline void print_order_vec(const kPlanarOnly&) { printf("Planar"); }
inline void print_order_vec(const kInterleavedAndPlanar&) { printf("InterleavedAndPlanar"); }

inline void print_step_vec(const kNonStepOnly&) { printf("NonStep"); }
inline void print_step_vec(const kStepOnly&) { printf("Step"); }
inline void print_step_vec(const kNonStepAndStep&) { printf("NonStepAndStep"); }


template <typename ChanVec, typename CsVec, typename OrderVec, typename StepVec, bool IsMutable>
void print_vec(const boost::mpl::cross_vector<boost::mpl::vector4<ChanVec, CsVec, OrderVec, StepVec>,
                                              typename cross_vector_image_view_types<ChanVec, CsVec, OrderVec, StepVec, IsMutable>::generate_fn>& ai) { 
    if (IsMutable)
        printf("Const");
    printf("ViewCrossVec of: ("); 
    print_vec(ChanVec()); printf(" ");
    print_vec(CsVec()); printf(" ");
    print_order_vec(OrderVec()); printf(" ");
    print_step_vec(StepVec());
    printf(")\n");
}

template <typename ChanVec, typename CsVec, typename OrderVec>
void print_vec(const boost::mpl::cross_vector<boost::mpl::vector3<ChanVec, CsVec, OrderVec>, 
               typename cross_vector_image_types<ChanVec, CsVec, OrderVec>::generate_fn>& ai) { 
    typedef boost::mpl::cross_vector<boost::mpl::vector3<ChanVec, CsVec, OrderVec>, typename cross_vector_image_types<ChanVec, CsVec, OrderVec>::generate_fn> T;
    printf("ImageCrossVec size:%d = (", boost::mpl::size<T>::value); 
    print_vec(ChanVec()); printf(" ");
    print_vec(CsVec()); printf(" ");
    print_order_vec(OrderVec());
    printf(")\n");
}

template <typename Types1, typename Types2>
void print_vec(const boost::mpl::concat_vector<Types1,Types2>& cv) { 
    typedef boost::mpl::concat_vector<Types1,Types2> T;
    printf("_______ ConcatVec size:%d = (",boost::mpl::size<T>::value);
    print_vec(Types1());
    print_vec(Types2());
    printf(")_______\n");
}

namespace detail {
    template <typename Iterator, int NumLeft> 
    struct print_set_iter {
        void operator()() {
            print(typename boost::mpl::deref<Iterator>::type());
            printf(" ");
            print_set_iter<typename boost::mpl::next<Iterator>::type, NumLeft-1>()();
        }
    };
    template <typename Iterator>
    struct print_set_iter<Iterator,1> {
        void operator()() {
            print(typename boost::mpl::deref<Iterator>::type());
            printf("]\n");
        }
    };
}

template <typename Types> 
void print_set(const Types&) {
    printf("(%d) [",boost::mpl::size<Types>::value);
    detail::print_set_iter<typename boost::mpl::begin<Types>::type, boost::mpl::size<Types>::value>()();
}

template <typename T1, typename T2> 
void print(const std::pair<T1,T2> p) {
    printf("pair<"); print(p.first); printf(","); print(p.second); printf("> ");
}

#ifdef GIL_REDUCE_CODE_BLOAT

template <typename Types,typename Op>
void print(const unary_reduce<Types,Op>&) {
    typedef unary_reduce<Types,Op> UR;
    printf("Types: "); print_vec(Types()); printf("\n");
    printf("REDUCED: "); print_vec(UR::reduced_t());printf("\n");
    printf("UNIQUE: "); print_set(UR::unique_t());printf("\n");
    printf("INDICES: "); print_vec(UR::indices_t());printf("\n");
};

template <typename RTypes1,typename RTypes2,typename Op>
void print(const detail::binary_reduce_impl<RTypes1,RTypes2,Op, true>&) {
    printf("type combinations (%d) are above limit - binary reduce suppressed \n", boost::mpl::size<RTypes1>::value * boost::mpl::size<RTypes2>::value);
};

template <typename RTypes1,typename RTypes2,typename Op>
void print(const detail::binary_reduce_impl<RTypes1,RTypes2,Op, false>&) {
    typedef detail::binary_reduce_impl<RTypes1,RTypes2,Op, false> BR;
    print(BR::bin_reduced_t());
};


template <typename Types1,typename Types2,typename BinOp>
void print(const binary_reduce<Types1,Types2,BinOp>&) {
    typedef binary_reduce<Types1,Types2,BinOp> BR;
    printf("_____ UNARY REDUCE ON Types1: ");
    print(BR::reduced1_t());
    printf("\n_____ BINARY REDUCE: ");
    print(BR::impl());
};
#endif

ADOBE_GIL_NAMESPACE_END

#endif
