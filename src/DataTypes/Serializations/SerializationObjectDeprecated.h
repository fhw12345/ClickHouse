#pragma once

#include <Columns/ColumnObjectDeprecated.h>
#include <DataTypes/Serializations/SimpleTextSerialization.h>
#include <Common/ObjectPool.h>

namespace DB
{

/** Serialization for data type Object (deprecated).
  * Supported only text serialization/deserialization.
  * and binary bulk serialization/deserialization without position independent
  * encoding, i.e. serialization/deserialization into Native format.
  */
template <typename Parser>
class SerializationObjectDeprecated : public ISerialization
{
public:
    /** In Native format ColumnObjectDeprecated can be serialized
      * in two formats: as Tuple or as String.
      * The format is the following:
      *
      * <serialization_kind> 1 byte -- 0 if Tuple, 1 if String.
      * [type_name] -- Only for tuple serialization.
      * ... data of internal column ...
      *
      * ClickHouse client serializazes objects as tuples.
      * String serialization exists for clients, which cannot
      * do parsing by themselves and they can send raw data as
      * string. It will be parsed on the server side.
      */

    void serializeBinaryBulkStatePrefix(
        const IColumn & column,
        SerializeBinaryBulkSettings & settings,
        SerializeBinaryBulkStatePtr & state) const override;

    void serializeBinaryBulkStateSuffix(
        SerializeBinaryBulkSettings & settings,
        SerializeBinaryBulkStatePtr & state) const override;

    void deserializeBinaryBulkStatePrefix(
        DeserializeBinaryBulkSettings & settings,
        DeserializeBinaryBulkStatePtr & state,
        SubstreamsDeserializeStatesCache * cache) const override;

    void serializeBinaryBulkWithMultipleStreams(
        const IColumn & column,
        size_t offset,
        size_t limit,
        SerializeBinaryBulkSettings & settings,
        SerializeBinaryBulkStatePtr & state) const override;

    void deserializeBinaryBulkWithMultipleStreams(
        ColumnPtr & column,
        size_t rows_offset,
        size_t limit,
        DeserializeBinaryBulkSettings & settings,
        DeserializeBinaryBulkStatePtr & state,
        SubstreamsCache * cache) const override;

    void serializeBinary(const Field & field, WriteBuffer & ostr, const FormatSettings &) const override;
    void deserializeBinary(Field & field, ReadBuffer & istr, const FormatSettings &) const override;
    void serializeBinary(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings &) const override;
    void deserializeBinary(IColumn & column, ReadBuffer & istr, const FormatSettings &) const override;

    void serializeText(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override;
    void serializeTextEscaped(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override;
    void serializeTextQuoted(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override;
    void serializeTextJSON(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override;
    void serializeTextJSONPretty(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings, size_t indent) const override;
    void serializeTextCSV(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override;
    void serializeTextMarkdown(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const override;

    void deserializeWholeText(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override;
    void deserializeTextEscaped(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override;
    void deserializeTextQuoted(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override;
    void deserializeTextJSON(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override;
    void deserializeTextCSV(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override;

private:
    enum class BinarySerializationKind : UInt8
    {
        TUPLE = 0,
        STRING = 1,
    };

    struct SerializeStateObject;
    struct DeserializeStateObject;

    void deserializeBinaryBulkFromString(
        ColumnObjectDeprecated & column_object,
        size_t rows_offset,
        size_t limit,
        DeserializeBinaryBulkSettings & settings,
        DeserializeStateObject & state,
        SubstreamsCache * cache) const;

    void deserializeBinaryBulkFromTuple(
        ColumnObjectDeprecated & column_object,
        size_t rows_offset,
        size_t limit,
        DeserializeBinaryBulkSettings & settings,
        DeserializeStateObject & state,
        SubstreamsCache * cache) const;

    template <typename TSettings>
    void checkSerializationIsSupported(const TSettings & settings) const;

    template <typename Reader>
    void deserializeTextImpl(IColumn & column, Reader && reader) const;

    void serializeTextImpl(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings) const;

    template <bool pretty_json = false>
    void serializeTextFromSubcolumn(const ColumnObjectDeprecated::Subcolumn & subcolumn, size_t row_num, WriteBuffer & ostr, const FormatSettings & settings, size_t indent = 0) const;

    /// Pool of parser objects to make SerializationObjectDeprecated thread safe.
    mutable SimpleObjectPool<Parser> parsers_pool;
};

SerializationPtr getObjectSerialization(const String & schema_format);

}
