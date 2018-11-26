////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SparseDataVector.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVector.h"
#include "IndexValue.h"

#ifndef SPARSEDATAVECTOR_H
#define SPARSEDATAVECTOR_H

#include <utilities/include/CompressedIntegerList.h>

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <vector>

namespace ell
{
namespace data
{
    // forward declaration of SparseDataVector
    template <typename ElementType, typename IndexListType>
    class SparseDataVector;

    // forward declaration of SparseDataVectorIterator
    template <IterationPolicy policy, typename ElementType, typename IndexListType>
    class SparseDataVectorIterator;

    /// <summary> A read-only forward iterator that traverses the non-zero elements. </summary>
    template <typename ElementType, typename IndexListType>
    class SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType> : public IIndexValueIterator
    {
    public:
        SparseDataVectorIterator(const SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>&) = default;

        SparseDataVectorIterator(SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>&&) = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const;

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns the current iterate. </summary>
        ///
        /// <returns> An IndexValue that represents the current iterate. </returns>
        IndexValue Get() const;

    private:
        // define typenames to improve readability
        using IndexIteratorType = typename IndexListType::Iterator;
        using ValueIteratorType = typename std::vector<ElementType>::const_iterator;

        // private ctor, can only be called from SparseDataVector and the dense Iterator classes
        SparseDataVectorIterator(const IndexIteratorType& list_iterator, const ValueIteratorType& value_iterator, size_t size);
        friend SparseDataVector<ElementType, IndexListType>;

        // members
        IndexIteratorType _indexIterator;
        ValueIteratorType _valueIterator;
        size_t _size;
    };

    /// <summary> A read-only forward iterator that traverses a prefix of the vector, including zero elements. </summary>
    template <typename ElementType, typename IndexListType>
    class SparseDataVectorIterator<IterationPolicy::all, ElementType, IndexListType> : public IIndexValueIterator
    {
    public:
        SparseDataVectorIterator(const SparseDataVectorIterator<IterationPolicy::all, ElementType, IndexListType>&) = default;

        SparseDataVectorIterator(SparseDataVectorIterator<IterationPolicy::all, ElementType, IndexListType>&&) = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const { return _index < _size; }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns the current iterate. </summary>
        ///
        /// <returns> An IndexValue that represents the current iterate. </returns>
        IndexValue Get() const;

    protected:
        using IndexIteratorType = typename IndexListType::Iterator;
        using ValueIteratorType = typename std::vector<ElementType>::const_iterator;

        // private ctor that can only be called from the containing class
        SparseDataVectorIterator(const IndexIteratorType& list_iterator, const ValueIteratorType& value_iterator, size_t size);
        friend SparseDataVector<ElementType, IndexListType>;

        IndexIteratorType _indexIterator;
        ValueIteratorType _valueIterator;
        size_t _iteratorIndex;
        size_t _size;
        size_t _index = 0;
    };

    /// <summary> Implements a sparse vector as an increasing list of indices and their corresponding values.
    ///
    /// <typeparam name="ElementType"> Type of the vector elements. </typeparam>
    /// <typeparam name="tegerListType"> Type of the integer list used to store indices. </typeparam>
    template <typename ElementType, typename IndexListType>
    class SparseDataVector : public DataVectorBase<SparseDataVector<ElementType, IndexListType>>
    {
    public:
        SparseDataVector() = default;

        SparseDataVector(SparseDataVector<ElementType, IndexListType>&& other) = default;

        SparseDataVector(const SparseDataVector<ElementType, IndexListType>& other) = delete;

        /// <summary> Constructs a DenseDataVector from an index value iterator. </summary>
        ///
        /// <typeparam name="SparseIteratorType"> Type of index value iterator. </typeparam>
        /// <param name="SparseIterator"> The index value iterator. </param>
        template <typename SparseIteratorType, IsIndexValueIterator<SparseIteratorType> Concept = true>
        SparseDataVector(SparseIteratorType SparseIterator);

        /// <summary> Constructs a data vector from an initializer list of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        SparseDataVector(std::initializer_list<IndexValue> list);

        /// <summary> Constructs a data vector from an initializer list of values. </summary>
        ///
        /// <param name="list"> The initializer list of values. </param>
        SparseDataVector(std::initializer_list<double> list);

        /// <summary> Constructs a data vector from a vector of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        SparseDataVector(std::vector<IndexValue> vec);

        /// <summary> Constructs a data vector from a vector of values. </summary>
        ///
        /// <param name="list"> The initializer list of values. </param>
        SparseDataVector(std::vector<double> vec);

        template <IterationPolicy policy>
        using Iterator = SparseDataVectorIterator<policy, ElementType, IndexListType>;

        /// <summary>
        /// Returns an indexValue iterator that points to the beginning of the vector, which iterates
        /// over a prefix of the vector.
        /// </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        /// <param name="size"> The prefix size. </param>
        ///
        /// <returns> The iterator. </returns>
        template <IterationPolicy policy>
        Iterator<policy> GetIterator(size_t size) const;

        /// <summary>
        /// Returns an indexValue iterator that points to the beginning of the vector, which iterates
        /// over a prefix of length PrefixLength().
        /// </summary>
        ///
        /// <typeparam name="policy"> The iteration policy. </typeparam>
        ///
        /// <returns> The iterator. </returns>
        template <IterationPolicy policy>
        Iterator<policy> GetIterator() const
        {
            return GetIterator<policy>(PrefixLength());
        }

        /// <summary> Appends an element to the end of the data vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the element, must be bigger than the biggest current index. </param>
        /// <param name="value"> The element value. </param>
        void AppendElement(size_t index, double value) override;

        /// <summary>
        /// A data vector has infinite dimension and ends with a suffix of zeros. This function returns
        /// the first index in this suffix. Equivalently, the returned value is one plus the index of the
        /// last non-zero element.
        /// </summary>
        ///
        /// <returns> The first index of the suffix of zeros at the end of this vector. </returns>
        size_t PrefixLength() const override;

        /// <summary> Gets the data vector type (implemented by template specialization). </summary>
        ///
        /// <returns> The data vector type. </returns>
        IDataVector::Type GetType() const override
        {
            return GetStaticType();
        }

        static IDataVector::Type GetStaticType();

    private:
        using DataVectorBase<SparseDataVector<ElementType, IndexListType>>::AppendElements;
        IndexListType _indexList;
        std::vector<ElementType> _values;
    };

    /// <summary> A sparse data vector with double elements. </summary>
    using SparseDoubleDataVector = SparseDataVector<double, utilities::CompressedIntegerList>;

    /// <summary> A sparse data vector with float elements. </summary>
    using SparseFloatDataVector = SparseDataVector<float, utilities::CompressedIntegerList>;

    /// <summary> A sparse data vector with short elements. </summary>
    using SparseShortDataVector = SparseDataVector<short, utilities::CompressedIntegerList>;

    /// <summary> A sparse data vector with byte elements. </summary>
    using SparseByteDataVector = SparseDataVector<char, utilities::CompressedIntegerList>;
} // namespace data
} // namespace ell

#pragma region implementation

#include <utilities/include/Exception.h>

namespace ell
{
namespace data
{

    template <typename ElementType, typename IndexListType>
    SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>::SparseDataVectorIterator(
        const IndexIteratorType& index_iterator,
        const ValueIteratorType& valueIterator,
        size_t size) :
        _indexIterator(index_iterator),
        _valueIterator(valueIterator),
        _size(size)
    {
    }
    template <typename ElementType, typename IndexListType>
    bool SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>::IsValid() const
    {
        return _indexIterator.IsValid() && _indexIterator.Get() < _size;
    }

    template <typename ElementType, typename IndexListType>
    void SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>::Next()
    {
        _indexIterator.Next();
        ++_valueIterator;
    }

    template <typename ElementType, typename IndexListType>
    IndexValue SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>::Get() const
    {
        return IndexValue{ _indexIterator.Get(), static_cast<double>(*_valueIterator) };
    }

    template <typename ElementType, typename IndexListType>
    void SparseDataVectorIterator<IterationPolicy::all, ElementType, IndexListType>::Next()
    {
        if (_index == _iteratorIndex)
        {
            _indexIterator.Next();
            ++_valueIterator;
            if (_indexIterator.IsValid() && _indexIterator.Get() < _size)
            {
                _iteratorIndex = _indexIterator.Get();
            }
            else
            {
                _iteratorIndex = _size;
            }
        }
        ++_index;
    }

    template <typename ElementType, typename IndexListType>
    IndexValue SparseDataVectorIterator<IterationPolicy::all, ElementType, IndexListType>::Get() const
    {
        if (_index == _iteratorIndex)
        {
            return IndexValue{ _index, static_cast<double>(*_valueIterator) };
        }
        return IndexValue{ _index, 0.0 };
    }

    template <typename ElementType, typename IndexListType>
    SparseDataVectorIterator<IterationPolicy::all, ElementType, IndexListType>::SparseDataVectorIterator(const IndexIteratorType& indexIterator, const ValueIteratorType& valueIterator, size_t size) :
        _indexIterator(indexIterator),
        _valueIterator(valueIterator),
        _size(size)
    {
        _iteratorIndex = _indexIterator.IsValid() ? _indexIterator.Get() : _size;
    }

    template <typename ElementType, typename IndexListType>
    template <typename SparseIteratorType, IsIndexValueIterator<SparseIteratorType> Concept>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(SparseIteratorType SparseIterator)
    {
        AppendElements(std::move(SparseIterator));
    }

    template <typename ElementType, typename IndexListType>
    template <IterationPolicy policy>
    auto SparseDataVector<ElementType, IndexListType>::GetIterator(size_t size) const -> Iterator<policy>
    {
        return Iterator<policy>(_indexList.GetIterator(), _values.cbegin(), size);
    }

    template <typename ElementType, typename IndexListType>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(std::initializer_list<IndexValue> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType, typename IndexListType>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(std::initializer_list<double> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType, typename IndexListType>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(std::vector<IndexValue> vec)
    {
        AppendElements(std::move(vec));
    }

    template <typename ElementType, typename IndexListType>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(std::vector<double> vec)
    {
        AppendElements(std::move(vec));
    }

    template <typename ElementType, typename IndexListType>
    void SparseDataVector<ElementType, IndexListType>::AppendElement(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        ElementType storedValue = static_cast<ElementType>(value);
        assert(storedValue - value <= 1.0e-6 && value - storedValue <= 1.0e-6);

        if (_indexList.Size() > 0)
        {
            if (index <= _indexList.Max())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only append values to the end of a data vector");
            }
        }

        _indexList.Append(index);
        _values.push_back(storedValue);
    }

    template <typename ElementType, typename IndexListType>
    size_t SparseDataVector<ElementType, IndexListType>::PrefixLength() const
    {
        if (_indexList.Size() == 0)
        {
            return 0;
        }
        else
        {
            return _indexList.Max() + 1;
        }
    }
} // namespace data
} // namespace ell

#pragma endregion implementation

#endif // SPARSEDATAVECTOR_H
