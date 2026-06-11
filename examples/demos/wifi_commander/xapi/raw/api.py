# C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api3/raw\api.py
# -*- coding: utf-8 -*-
# PyXB bindings for NM:e92452c8d3e28a9e27abfc9994d2007779e7f4c9
# Generated 2024-02-16 15:27:13.154571 by PyXB version 1.2.6 using Python 3.8.2.final.0
# Namespace AbsentNamespace0

from __future__ import unicode_literals
import pyxb
import pyxb.binding
import pyxb.binding.saxer
import io
import pyxb.utils.utility
import pyxb.utils.domutils
import sys
import pyxb.utils.six as _six
# Unique identifier for bindings created at the same time
_GenerationUID = pyxb.utils.utility.UniqueIdentifier('urn:uuid:79eddab7-ccd7-11ee-8931-a87eeab69d6e')

# Version of PyXB used to generate the bindings
_PyXBVersion = '1.2.6'
# Generated bindings are not compatible across PyXB versions
if pyxb.__version__ != _PyXBVersion:
    raise pyxb.PyXBVersionError(_PyXBVersion)

# A holder for module-level binding classes so we can access them from
# inside class definitions where property names may conflict.
_module_typeBindings = pyxb.utils.utility.Object()

# Import bindings for namespaces imported into schema
import pyxb.binding.datatypes

# NOTE: All namespace declarations are reserved within the binding
Namespace = pyxb.namespace.CreateAbsentNamespace()
Namespace.configureCategories(['typeBinding', 'elementBinding'])

def CreateFromDocument (xml_text, default_namespace=None, location_base=None):
    """Parse the given XML and use the document element to create a
    Python instance.

    @param xml_text An XML document.  This should be data (Python 2
    str or Python 3 bytes), or a text (Python 2 unicode or Python 3
    str) in the L{pyxb._InputEncoding} encoding.

    @keyword default_namespace The L{pyxb.Namespace} instance to use as the
    default namespace where there is no default namespace in scope.
    If unspecified or C{None}, the namespace of the module containing
    this function will be used.

    @keyword location_base: An object to be recorded as the base of all
    L{pyxb.utils.utility.Location} instances associated with events and
    objects handled by the parser.  You might pass the URI from which
    the document was obtained.
    """

    if pyxb.XMLStyle_saxer != pyxb._XMLStyle:
        dom = pyxb.utils.domutils.StringToDOM(xml_text)
        return CreateFromDOM(dom.documentElement, default_namespace=default_namespace)
    if default_namespace is None:
        default_namespace = Namespace.fallbackNamespace()
    saxer = pyxb.binding.saxer.make_parser(fallback_namespace=default_namespace, location_base=location_base)
    handler = saxer.getContentHandler()
    xmld = xml_text
    if isinstance(xmld, _six.text_type):
        xmld = xmld.encode(pyxb._InputEncoding)
    saxer.parse(io.BytesIO(xmld))
    instance = handler.rootObject()
    return instance

def CreateFromDOM (node, default_namespace=None):
    """Create a Python instance from the given DOM node.
    The node tag must correspond to an element declaration in this module.

    @deprecated: Forcing use of DOM interface is unnecessary; use L{CreateFromDocument}."""
    if default_namespace is None:
        default_namespace = Namespace.fallbackNamespace()
    return pyxb.binding.basis.element.AnyCreateFromDOM(node, default_namespace)


# Atomic simple type: [anonymous]
class STD_ANON (pyxb.binding.datatypes.string, pyxb.binding.basis.enumeration_mixin):

    """An atomic simple type."""

    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 188, 3)
    _Documentation = None
STD_ANON._CF_enumeration = pyxb.binding.facets.CF_enumeration(value_datatype=STD_ANON, enum_prefix=None)
STD_ANON.int8 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='int8', tag='int8')
STD_ANON.int16 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='int16', tag='int16')
STD_ANON.int32 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='int32', tag='int32')
STD_ANON.uint8 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='uint8', tag='uint8')
STD_ANON.uint16 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='uint16', tag='uint16')
STD_ANON.uint32 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='uint32', tag='uint32')
STD_ANON.uint8array = STD_ANON._CF_enumeration.addEnumeration(unicode_value='uint8array', tag='uint8array')
STD_ANON.uint16array = STD_ANON._CF_enumeration.addEnumeration(unicode_value='uint16array', tag='uint16array')
STD_ANON.byte_array = STD_ANON._CF_enumeration.addEnumeration(unicode_value='byte_array', tag='byte_array')
STD_ANON.string = STD_ANON._CF_enumeration.addEnumeration(unicode_value='string', tag='string')
STD_ANON.bd_addr = STD_ANON._CF_enumeration.addEnumeration(unicode_value='bd_addr', tag='bd_addr')
STD_ANON.ipv4_addr = STD_ANON._CF_enumeration.addEnumeration(unicode_value='ipv4_addr', tag='ipv4_addr')
STD_ANON.ipv6_addr = STD_ANON._CF_enumeration.addEnumeration(unicode_value='ipv6_addr', tag='ipv6_addr')
STD_ANON.mac_addr = STD_ANON._CF_enumeration.addEnumeration(unicode_value='mac_addr', tag='mac_addr')
STD_ANON.ser_name = STD_ANON._CF_enumeration.addEnumeration(unicode_value='ser_name', tag='ser_name')
STD_ANON.uuid_128 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='uuid_128', tag='uuid_128')
STD_ANON.aes_key_128 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='aes_key_128', tag='aes_key_128')
STD_ANON.app_wifi_uuid_16_t = STD_ANON._CF_enumeration.addEnumeration(unicode_value='app_wifi_uuid_16_t', tag='app_wifi_uuid_16_t')
STD_ANON.app_wifi_uuid_64_t = STD_ANON._CF_enumeration.addEnumeration(unicode_value='app_wifi_uuid_64_t', tag='app_wifi_uuid_64_t')
STD_ANON.app_bt_uuid_t = STD_ANON._CF_enumeration.addEnumeration(unicode_value='app_bt_uuid_t', tag='app_bt_uuid_t')
STD_ANON.int64 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='int64', tag='int64')
STD_ANON.uint64 = STD_ANON._CF_enumeration.addEnumeration(unicode_value='uint64', tag='uint64')
STD_ANON._InitializeFacetMap(STD_ANON._CF_enumeration)
_module_typeBindings.STD_ANON = STD_ANON

# Atomic simple type: [anonymous]
class STD_ANON_ (pyxb.binding.datatypes.string, pyxb.binding.basis.enumeration_mixin):

    """An atomic simple type."""

    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 257, 3)
    _Documentation = None
STD_ANON_._CF_enumeration = pyxb.binding.facets.CF_enumeration(value_datatype=STD_ANON_, enum_prefix=None)
STD_ANON_.internal = STD_ANON_._CF_enumeration.addEnumeration(unicode_value='internal', tag='internal')
STD_ANON_.unpublished = STD_ANON_._CF_enumeration.addEnumeration(unicode_value='unpublished', tag='unpublished')
STD_ANON_.restricted = STD_ANON_._CF_enumeration.addEnumeration(unicode_value='restricted', tag='restricted')
STD_ANON_.public = STD_ANON_._CF_enumeration.addEnumeration(unicode_value='public', tag='public')
STD_ANON_._InitializeFacetMap(STD_ANON_._CF_enumeration)
_module_typeBindings.STD_ANON_ = STD_ANON_

# Atomic simple type: [anonymous]
class STD_ANON_2 (pyxb.binding.datatypes.string):

    """An atomic simple type."""

    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 267, 3)
    _Documentation = None
STD_ANON_2._CF_pattern = pyxb.binding.facets.CF_pattern()
STD_ANON_2._CF_pattern.addPattern(pattern='(\\w+-)?v\\d+\\.\\d+\\.\\d+')
STD_ANON_2._InitializeFacetMap(STD_ANON_2._CF_pattern)
_module_typeBindings.STD_ANON_2 = STD_ANON_2

# Atomic simple type: [anonymous]
class STD_ANON_3 (pyxb.binding.datatypes.string):

    """An atomic simple type."""

    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 274, 3)
    _Documentation = None
STD_ANON_3._CF_pattern = pyxb.binding.facets.CF_pattern()
STD_ANON_3._CF_pattern.addPattern(pattern='(\\w+-)?v\\d+\\.\\d+\\.\\d+')
STD_ANON_3._InitializeFacetMap(STD_ANON_3._CF_pattern)
_module_typeBindings.STD_ANON_3 = STD_ANON_3

# Atomic simple type: [anonymous]
class STD_ANON_4 (pyxb.binding.datatypes.string):

    """An atomic simple type."""

    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 281, 3)
    _Documentation = None
STD_ANON_4._CF_pattern = pyxb.binding.facets.CF_pattern()
STD_ANON_4._CF_pattern.addPattern(pattern='(\\w+-)?v\\d+\\.\\d+\\.\\d+')
STD_ANON_4._InitializeFacetMap(STD_ANON_4._CF_pattern)
_module_typeBindings.STD_ANON_4 = STD_ANON_4

# Complex type returns with content type ELEMENT_ONLY
class returns (pyxb.binding.basis.complexTypeDefinition):
    """Complex type returns with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'returns')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 5, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element param uses Python identifier param
    __param = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'param'), 'param', '__AbsentNamespace0_returns_param', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 7, 3), )

    
    param = property(__param.value, __param.set, None, None)

    _ElementMap.update({
        __param.name() : __param
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.returns = returns
Namespace.addCategoryObject('typeBinding', 'returns', returns)


# Complex type params with content type ELEMENT_ONLY
class params (pyxb.binding.basis.complexTypeDefinition):
    """Complex type params with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'params')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 10, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element param uses Python identifier param
    __param = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'param'), 'param', '__AbsentNamespace0_params_param', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 12, 3), )

    
    param = property(__param.value, __param.set, None, None)

    _ElementMap.update({
        __param.name() : __param
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.params = params
Namespace.addCategoryObject('typeBinding', 'params', params)


# Complex type api with content type ELEMENT_ONLY
class api_ (pyxb.binding.basis.complexTypeDefinition):
    """Complex type api with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'api')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 129, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element template_context uses Python identifier template_context
    __template_context = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'template_context'), 'template_context', '__AbsentNamespace0_api__template_context', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 131, 3), )

    
    template_context = property(__template_context.value, __template_context.set, None, '\n\t\t\t\t\t\tThe content of this element is evaluated by Jinja2 before rendering any\n\t\t\t\t\t\ttemplated descriptions or conditions. It can be used to define variables\n\t\t\t\t\t\tor macros that are used later in the templated content.\n\t\t\t\t\t')

    
    # Element types uses Python identifier types
    __types = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'types'), 'types', '__AbsentNamespace0_api__types', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 140, 3), )

    
    types = property(__types.value, __types.set, None, 'Type aliases for providing default description where more specific one is omitted')

    
    # Element overviewdoc uses Python identifier overviewdoc
    __overviewdoc = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'overviewdoc'), 'overviewdoc', '__AbsentNamespace0_api__overviewdoc', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 149, 3), )

    
    overviewdoc = property(__overviewdoc.value, __overviewdoc.set, None, None)

    
    # Element class uses Python identifier class_
    __class = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'class'), 'class_', '__AbsentNamespace0_api__class', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 150, 3), )

    
    class_ = property(__class.value, __class.set, None, None)

    
    # Attribute device_id uses Python identifier device_id
    __device_id = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'device_id'), 'device_id', '__AbsentNamespace0_api__device_id', pyxb.binding.datatypes.nonNegativeInteger)
    __device_id._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 161, 2)
    __device_id._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 161, 2)
    
    device_id = property(__device_id.value, __device_id.set, None, None)

    
    # Attribute device_name uses Python identifier device_name
    __device_name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'device_name'), 'device_name', '__AbsentNamespace0_api__device_name', pyxb.binding.datatypes.string)
    __device_name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 162, 2)
    __device_name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 162, 2)
    
    device_name = property(__device_name.value, __device_name.set, None, None)

    _ElementMap.update({
        __template_context.name() : __template_context,
        __types.name() : __types,
        __overviewdoc.name() : __overviewdoc,
        __class.name() : __class
    })
    _AttributeMap.update({
        __device_id.name() : __device_id,
        __device_name.name() : __device_name
    })
_module_typeBindings.api_ = api_
Namespace.addCategoryObject('typeBinding', 'api', api_)


# Complex type types with content type ELEMENT_ONLY
class types (pyxb.binding.basis.complexTypeDefinition):
    """Complex type types with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'types')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 174, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element type uses Python identifier type
    __type = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'type'), 'type', '__AbsentNamespace0_types_type', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 176, 3), )

    
    type = property(__type.value, __type.set, None, None)

    _ElementMap.update({
        __type.name() : __type
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.types = types
Namespace.addCategoryObject('typeBinding', 'types', types)


# Complex type events with content type ELEMENT_ONLY
class events (pyxb.binding.basis.complexTypeDefinition):
    """Complex type events with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'events')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 227, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element event uses Python identifier event
    __event = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'event'), 'event', '__AbsentNamespace0_events_event', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 229, 3), )

    
    event = property(__event.value, __event.set, None, None)

    _ElementMap.update({
        __event.name() : __event
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.events = events
Namespace.addCategoryObject('typeBinding', 'events', events)


# Complex type overviewdoc with content type ELEMENT_ONLY
class overviewdoc (pyxb.binding.basis.complexTypeDefinition):
    """Complex type overviewdoc with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'overviewdoc')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 240, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element section uses Python identifier section
    __section = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'section'), 'section', '__AbsentNamespace0_overviewdoc_section', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 242, 3), )

    
    section = property(__section.value, __section.set, None, None)

    
    # Attribute title uses Python identifier title
    __title = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'title'), 'title', '__AbsentNamespace0_overviewdoc_title', pyxb.binding.datatypes.string, required=True)
    __title._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 244, 2)
    __title._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 244, 2)
    
    title = property(__title.value, __title.set, None, None)

    _ElementMap.update({
        __section.name() : __section
    })
    _AttributeMap.update({
        __title.name() : __title
    })
_module_typeBindings.overviewdoc = overviewdoc
Namespace.addCategoryObject('typeBinding', 'overviewdoc', overviewdoc)


# Complex type param with content type MIXED
class param (pyxb.binding.basis.complexTypeDefinition):
    """Complex type param with content type MIXED"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_MIXED
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'param')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 15, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element value uses Python identifier value_
    __value = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'value'), 'value_', '__AbsentNamespace0_param_value', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 18, 3), )

    
    value_ = property(__value.value, __value.set, None, None)

    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_param_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_param_name', pyxb.binding.datatypes.string, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 20, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 20, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute type uses Python identifier type
    __type = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'type'), 'type', '__AbsentNamespace0_param_type', pyxb.binding.datatypes.string, required=True)
    __type._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 21, 2)
    __type._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 21, 2)
    
    type = property(__type.value, __type.set, None, None)

    
    # Attribute enum uses Python identifier enum
    __enum = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'enum'), 'enum', '__AbsentNamespace0_param_enum', pyxb.binding.datatypes.anySimpleType)
    __enum._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 23, 2)
    __enum._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 23, 2)
    
    enum = property(__enum.value, __enum.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_param_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_param_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_param_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_param_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_param_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __value.name() : __value,
        __description.name() : __description
    })
    _AttributeMap.update({
        __name.name() : __name,
        __type.name() : __type,
        __enum.name() : __enum,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.param = param
Namespace.addCategoryObject('typeBinding', 'param', param)


# Complex type event with content type ELEMENT_ONLY
class event (pyxb.binding.basis.complexTypeDefinition):
    """Complex type event with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'event')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 25, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element params uses Python identifier params
    __params = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'params'), 'params', '__AbsentNamespace0_event_params', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 28, 3), )

    
    params = property(__params.value, __params.set, None, None)

    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_event_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_event_name', pyxb.binding.datatypes.anySimpleType, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 30, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 30, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute title uses Python identifier title
    __title = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'title'), 'title', '__AbsentNamespace0_event_title', pyxb.binding.datatypes.string)
    __title._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 31, 2)
    __title._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 31, 2)
    
    title = property(__title.value, __title.set, None, None)

    
    # Attribute index uses Python identifier index
    __index = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'index'), 'index', '__AbsentNamespace0_event_index', pyxb.binding.datatypes.string, required=True)
    __index._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 33, 2)
    __index._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 33, 2)
    
    index = property(__index.value, __index.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_event_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_event_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_event_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_event_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_event_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __params.name() : __params,
        __description.name() : __description
    })
    _AttributeMap.update({
        __name.name() : __name,
        __title.name() : __title,
        __index.name() : __index,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.event = event
Namespace.addCategoryObject('typeBinding', 'event', event)


# Complex type enums with content type ELEMENT_ONLY
class enums (pyxb.binding.basis.complexTypeDefinition):
    """Complex type enums with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'enums')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 35, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element enum uses Python identifier enum
    __enum = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'enum'), 'enum', '__AbsentNamespace0_enums_enum', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 38, 3), )

    
    enum = property(__enum.value, __enum.set, None, None)

    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_enums_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Attribute compatibility uses Python identifier compatibility
    __compatibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'compatibility'), 'compatibility', '__AbsentNamespace0_enums_compatibility', pyxb.binding.datatypes.boolean, unicode_default='true')
    __compatibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 40, 2)
    __compatibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 40, 2)
    
    compatibility = property(__compatibility.value, __compatibility.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_enums_name', pyxb.binding.datatypes.anySimpleType, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 41, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 41, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute title uses Python identifier title
    __title = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'title'), 'title', '__AbsentNamespace0_enums_title', pyxb.binding.datatypes.anySimpleType, required=True)
    __title._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 42, 2)
    __title._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 42, 2)
    
    title = property(__title.value, __title.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_enums_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_enums_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_enums_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_enums_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_enums_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __enum.name() : __enum,
        __description.name() : __description
    })
    _AttributeMap.update({
        __compatibility.name() : __compatibility,
        __name.name() : __name,
        __title.name() : __title,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.enums = enums
Namespace.addCategoryObject('typeBinding', 'enums', enums)


# Complex type enum with content type ELEMENT_ONLY
class enum (pyxb.binding.basis.complexTypeDefinition):
    """Complex type enum with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'enum')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 45, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_enum_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_enum_name', pyxb.binding.datatypes.anySimpleType, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 49, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 49, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute title uses Python identifier title
    __title = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'title'), 'title', '__AbsentNamespace0_enum_title', pyxb.binding.datatypes.string)
    __title._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 50, 2)
    __title._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 50, 2)
    
    title = property(__title.value, __title.set, None, None)

    
    # Attribute value uses Python identifier value_
    __value = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'value'), 'value_', '__AbsentNamespace0_enum_value', pyxb.binding.datatypes.anySimpleType, required=True)
    __value._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 51, 2)
    __value._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 51, 2)
    
    value_ = property(__value.value, __value.set, None, 'Could also be optional, but since currently every enum provides values, automatic value creation is not implemented.')

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_enum_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_enum_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_enum_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_enum_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_enum_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __description.name() : __description
    })
    _AttributeMap.update({
        __name.name() : __name,
        __title.name() : __title,
        __value.name() : __value,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.enum = enum
Namespace.addCategoryObject('typeBinding', 'enum', enum)


# Complex type value with content type ELEMENT_ONLY
class value (pyxb.binding.basis.complexTypeDefinition):
    """Complex type value with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'value')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 58, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_value_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Attribute value uses Python identifier value_
    __value = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'value'), 'value_', '__AbsentNamespace0_value_value', pyxb.binding.datatypes.anySimpleType, required=True)
    __value._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 62, 2)
    __value._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 62, 2)
    
    value_ = property(__value.value, __value.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_value_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_value_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_value_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_value_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_value_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __description.name() : __description
    })
    _AttributeMap.update({
        __value.name() : __value,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.value = value
Namespace.addCategoryObject('typeBinding', 'value', value)


# Complex type defines with content type ELEMENT_ONLY
class defines (pyxb.binding.basis.complexTypeDefinition):
    """Complex type defines with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'defines')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 66, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_defines_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Element define uses Python identifier define
    __define = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'define'), 'define', '__AbsentNamespace0_defines_define', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 69, 3), )

    
    define = property(__define.value, __define.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_defines_name', pyxb.binding.datatypes.anySimpleType, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 71, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 71, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute title uses Python identifier title
    __title = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'title'), 'title', '__AbsentNamespace0_defines_title', pyxb.binding.datatypes.anySimpleType, required=True)
    __title._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 72, 2)
    __title._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 72, 2)
    
    title = property(__title.value, __title.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_defines_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_defines_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_defines_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_defines_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_defines_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __description.name() : __description,
        __define.name() : __define
    })
    _AttributeMap.update({
        __name.name() : __name,
        __title.name() : __title,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.defines = defines
Namespace.addCategoryObject('typeBinding', 'defines', defines)


# Complex type define with content type ELEMENT_ONLY
class define (pyxb.binding.basis.complexTypeDefinition):
    """Complex type define with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'define')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 75, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_define_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_define_name', pyxb.binding.datatypes.anySimpleType, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 79, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 79, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute value uses Python identifier value_
    __value = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'value'), 'value_', '__AbsentNamespace0_define_value', pyxb.binding.datatypes.anySimpleType, required=True)
    __value._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 80, 2)
    __value._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 80, 2)
    
    value_ = property(__value.value, __value.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_define_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_define_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_define_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_define_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_define_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __description.name() : __description
    })
    _AttributeMap.update({
        __name.name() : __name,
        __value.name() : __value,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.define = define
Namespace.addCategoryObject('typeBinding', 'define', define)


# Complex type command with content type ELEMENT_ONLY
class command (pyxb.binding.basis.complexTypeDefinition):
    """Complex type command with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'command')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 83, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_command_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Element params uses Python identifier params
    __params = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'params'), 'params', '__AbsentNamespace0_command_params', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 86, 3), )

    
    params = property(__params.value, __params.set, None, None)

    
    # Element returns uses Python identifier returns
    __returns = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'returns'), 'returns', '__AbsentNamespace0_command_returns', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 92, 3), )

    
    returns = property(__returns.value, __returns.set, None, None)

    
    # Element events uses Python identifier events
    __events = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'events'), 'events', '__AbsentNamespace0_command_events', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 98, 3), )

    
    events = property(__events.value, __events.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_command_name', pyxb.binding.datatypes.anySimpleType, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 100, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 100, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute title uses Python identifier title
    __title = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'title'), 'title', '__AbsentNamespace0_command_title', pyxb.binding.datatypes.string)
    __title._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 101, 2)
    __title._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 101, 2)
    
    title = property(__title.value, __title.set, None, None)

    
    # Attribute index uses Python identifier index
    __index = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'index'), 'index', '__AbsentNamespace0_command_index', pyxb.binding.datatypes.string, required=True)
    __index._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 103, 2)
    __index._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 103, 2)
    
    index = property(__index.value, __index.set, None, None)

    
    # Attribute no_return uses Python identifier no_return
    __no_return = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'no_return'), 'no_return', '__AbsentNamespace0_command_no_return', pyxb.binding.datatypes.boolean, unicode_default='false')
    __no_return._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 104, 2)
    __no_return._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 104, 2)
    
    no_return = property(__no_return.value, __no_return.set, None, None)

    
    # Attribute sensitive uses Python identifier sensitive
    __sensitive = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'sensitive'), 'sensitive', '__AbsentNamespace0_command_sensitive', pyxb.binding.datatypes.boolean, unicode_default='false')
    __sensitive._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 105, 2)
    __sensitive._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 105, 2)
    
    sensitive = property(__sensitive.value, __sensitive.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_command_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_command_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_command_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_command_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_command_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __description.name() : __description,
        __params.name() : __params,
        __returns.name() : __returns,
        __events.name() : __events
    })
    _AttributeMap.update({
        __name.name() : __name,
        __title.name() : __title,
        __index.name() : __index,
        __no_return.name() : __no_return,
        __sensitive.name() : __sensitive,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.command = command
Namespace.addCategoryObject('typeBinding', 'command', command)


# Complex type class with content type ELEMENT_ONLY
class class_ (pyxb.binding.basis.complexTypeDefinition):
    """Complex type class with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'class')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 107, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_class__description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Element command uses Python identifier command
    __command = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'command'), 'command', '__AbsentNamespace0_class__command', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 111, 4), )

    
    command = property(__command.value, __command.set, None, None)

    
    # Element event uses Python identifier event
    __event = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'event'), 'event', '__AbsentNamespace0_class__event', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 112, 4), )

    
    event = property(__event.value, __event.set, None, None)

    
    # Element enums uses Python identifier enums
    __enums = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'enums'), 'enums', '__AbsentNamespace0_class__enums', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 113, 4), )

    
    enums = property(__enums.value, __enums.set, None, None)

    
    # Element defines uses Python identifier defines
    __defines = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'defines'), 'defines', '__AbsentNamespace0_class__defines', True, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 119, 4), )

    
    defines = property(__defines.value, __defines.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_class__name', pyxb.binding.datatypes.anySimpleType, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 122, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 122, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute title uses Python identifier title
    __title = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'title'), 'title', '__AbsentNamespace0_class__title', pyxb.binding.datatypes.anySimpleType, required=True)
    __title._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 123, 2)
    __title._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 123, 2)
    
    title = property(__title.value, __title.set, None, None)

    
    # Attribute index uses Python identifier index
    __index = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'index'), 'index', '__AbsentNamespace0_class__index', pyxb.binding.datatypes.string, required=True)
    __index._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 124, 2)
    __index._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 124, 2)
    
    index = property(__index.value, __index.set, None, None)

    
    # Attribute feature uses Python identifier feature
    __feature = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'feature'), 'feature', '__AbsentNamespace0_class__feature', pyxb.binding.datatypes.string)
    __feature._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 126, 2)
    __feature._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 126, 2)
    
    feature = property(__feature.value, __feature.set, None, None)

    
    # Attribute layer uses Python identifier layer
    __layer = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'layer'), 'layer', '__AbsentNamespace0_class__layer', pyxb.binding.datatypes.string)
    __layer._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 127, 2)
    __layer._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 127, 2)
    
    layer = property(__layer.value, __layer.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_class__condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_class__visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_class__since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_class__deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_class__removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __description.name() : __description,
        __command.name() : __command,
        __event.name() : __event,
        __enums.name() : __enums,
        __defines.name() : __defines
    })
    _AttributeMap.update({
        __name.name() : __name,
        __title.name() : __title,
        __index.name() : __index,
        __feature.name() : __feature,
        __layer.name() : __layer,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.class_ = class_
Namespace.addCategoryObject('typeBinding', 'class', class_)


# Complex type type with content type ELEMENT_ONLY
class type (pyxb.binding.basis.complexTypeDefinition):
    """Complex type type with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'type')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 179, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_type_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_type_name', pyxb.binding.datatypes.string, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 183, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 183, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute datatype uses Python identifier datatype
    __datatype = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'datatype'), 'datatype', '__AbsentNamespace0_type_datatype', _module_typeBindings.STD_ANON, required=True)
    __datatype._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 184, 2)
    __datatype._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 184, 2)
    
    datatype = property(__datatype.value, __datatype.set, None, 'Must be one of basic types from x_types.h')

    
    # Attribute length uses Python identifier length
    __length = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'length'), 'length', '__AbsentNamespace0_type_length', pyxb.binding.datatypes.integer, required=True)
    __length._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 212, 2)
    __length._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 212, 2)
    
    length = property(__length.value, __length.set, None, 'Minimum length in bytes')

    
    # Attribute c_datatype uses Python identifier c_datatype
    __c_datatype = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'c_datatype'), 'c_datatype', '__AbsentNamespace0_type_c_datatype', pyxb.binding.datatypes.string, unicode_default='')
    __c_datatype._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 217, 2)
    __c_datatype._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 217, 2)
    
    c_datatype = property(__c_datatype.value, __c_datatype.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_type_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_type_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_type_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_type_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_type_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __description.name() : __description
    })
    _AttributeMap.update({
        __name.name() : __name,
        __datatype.name() : __datatype,
        __length.name() : __length,
        __c_datatype.name() : __c_datatype,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.type = type
Namespace.addCategoryObject('typeBinding', 'type', type)


# Complex type eventpointer with content type ELEMENT_ONLY
class eventpointer (pyxb.binding.basis.complexTypeDefinition):
    """Complex type eventpointer with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'eventpointer')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 220, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element description uses Python identifier description
    __description = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(Namespace, 'description'), 'description', '__AbsentNamespace0_eventpointer_description', False, pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1), )

    
    description = property(__description.value, __description.set, None, None)

    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_eventpointer_name', pyxb.binding.datatypes.anySimpleType, required=True)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 224, 2)
    __name._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 224, 2)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_eventpointer_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_eventpointer_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_eventpointer_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_eventpointer_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_eventpointer_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        __description.name() : __description
    })
    _AttributeMap.update({
        __name.name() : __name,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.eventpointer = eventpointer
Namespace.addCategoryObject('typeBinding', 'eventpointer', eventpointer)


# Complex type section with content type SIMPLE
class section (pyxb.binding.basis.complexTypeDefinition):
    """Complex type section with content type SIMPLE"""
    _TypeDefinition = pyxb.binding.datatypes.string
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_SIMPLE
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'section')
    _XSDLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 232, 1)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.string
    
    # Attribute title uses Python identifier title
    __title = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'title'), 'title', '__AbsentNamespace0_section_title', pyxb.binding.datatypes.string, required=True)
    __title._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 235, 4)
    __title._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 235, 4)
    
    title = property(__title.value, __title.set, None, None)

    
    # Attribute condition uses Python identifier condition
    __condition = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'condition'), 'condition', '__AbsentNamespace0_section_condition', pyxb.binding.datatypes.string)
    __condition._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    __condition._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 247, 2)
    
    condition = property(__condition.value, __condition.set, None, '\n\t\t\t\t\tWhen this attribute is specified, its content is evaluated inside a Jinja2\n\t\t\t\t\tconditional to decide whether the condition is true or false. If false, the\n\t\t\t\t\telement will be excluded from the tree.\n\t\t\t\t')

    
    # Attribute visibility uses Python identifier visibility
    __visibility = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'visibility'), 'visibility', '__AbsentNamespace0_section_visibility', _module_typeBindings.STD_ANON_, unicode_default='public')
    __visibility._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    __visibility._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 256, 2)
    
    visibility = property(__visibility.value, __visibility.set, None, None)

    
    # Attribute since uses Python identifier since
    __since = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'since'), 'since', '__AbsentNamespace0_section_since', _module_typeBindings.STD_ANON_2)
    __since._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    __since._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 266, 2)
    
    since = property(__since.value, __since.set, None, None)

    
    # Attribute deprecated uses Python identifier deprecated
    __deprecated = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'deprecated'), 'deprecated', '__AbsentNamespace0_section_deprecated', _module_typeBindings.STD_ANON_3)
    __deprecated._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    __deprecated._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 273, 2)
    
    deprecated = property(__deprecated.value, __deprecated.set, None, None)

    
    # Attribute removed uses Python identifier removed
    __removed = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'removed'), 'removed', '__AbsentNamespace0_section_removed', _module_typeBindings.STD_ANON_4)
    __removed._DeclarationLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    __removed._UseLocation = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 280, 2)
    
    removed = property(__removed.value, __removed.set, None, None)

    _ElementMap.update({
        
    })
    _AttributeMap.update({
        __title.name() : __title,
        __condition.name() : __condition,
        __visibility.name() : __visibility,
        __since.name() : __since,
        __deprecated.name() : __deprecated,
        __removed.name() : __removed
    })
_module_typeBindings.section = section
Namespace.addCategoryObject('typeBinding', 'section', section)


description = pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1))
Namespace.addCategoryObject('elementBinding', description.name().localName(), description)

api = pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'api'), api_, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 164, 1))
Namespace.addCategoryObject('elementBinding', api.name().localName(), api)



returns._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'param'), param, scope=returns, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 7, 3)))

def _BuildAutomaton ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton
    del _BuildAutomaton
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=None, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 7, 3))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(returns._UseForTag(pyxb.namespace.ExpandedName(None, 'param')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 7, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
returns._Automaton = _BuildAutomaton()




params._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'param'), param, scope=params, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 12, 3)))

def _BuildAutomaton_ ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_
    del _BuildAutomaton_
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=None, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 12, 3))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(params._UseForTag(pyxb.namespace.ExpandedName(None, 'param')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 12, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
params._Automaton = _BuildAutomaton_()




api_._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'template_context'), pyxb.binding.datatypes.string, scope=api_, documentation='\n\t\t\t\t\t\tThe content of this element is evaluated by Jinja2 before rendering any\n\t\t\t\t\t\ttemplated descriptions or conditions. It can be used to define variables\n\t\t\t\t\t\tor macros that are used later in the templated content.\n\t\t\t\t\t', location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 131, 3)))

api_._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'types'), types, scope=api_, documentation='Type aliases for providing default description where more specific one is omitted', location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 140, 3)))

api_._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'overviewdoc'), overviewdoc, scope=api_, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 149, 3)))

api_._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'class'), class_, scope=api_, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 150, 3)))

def _BuildAutomaton_2 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_2
    del _BuildAutomaton_2
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 131, 3))
    counters.add(cc_0)
    cc_1 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 140, 3))
    counters.add(cc_1)
    cc_2 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 149, 3))
    counters.add(cc_2)
    cc_3 = fac.CounterCondition(min=0, max=None, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 150, 3))
    counters.add(cc_3)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(api_._UseForTag(pyxb.namespace.ExpandedName(None, 'template_context')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 131, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_1, False))
    symbol = pyxb.binding.content.ElementUse(api_._UseForTag(pyxb.namespace.ExpandedName(None, 'types')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 140, 3))
    st_1 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_1)
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_2, False))
    symbol = pyxb.binding.content.ElementUse(api_._UseForTag(pyxb.namespace.ExpandedName(None, 'overviewdoc')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 149, 3))
    st_2 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_2)
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_3, False))
    symbol = pyxb.binding.content.ElementUse(api_._UseForTag(pyxb.namespace.ExpandedName(None, 'class')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 150, 3))
    st_3 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_3)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_0, False) ]))
    transitions.append(fac.Transition(st_2, [
        fac.UpdateInstruction(cc_0, False) ]))
    transitions.append(fac.Transition(st_3, [
        fac.UpdateInstruction(cc_0, False) ]))
    st_0._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_2, [
        fac.UpdateInstruction(cc_1, False) ]))
    transitions.append(fac.Transition(st_3, [
        fac.UpdateInstruction(cc_1, False) ]))
    st_1._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_2, [
        fac.UpdateInstruction(cc_2, True) ]))
    transitions.append(fac.Transition(st_3, [
        fac.UpdateInstruction(cc_2, False) ]))
    st_2._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_3, [
        fac.UpdateInstruction(cc_3, True) ]))
    st_3._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
api_._Automaton = _BuildAutomaton_2()




types._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'type'), type, scope=types, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 176, 3)))

def _BuildAutomaton_3 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_3
    del _BuildAutomaton_3
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(types._UseForTag(pyxb.namespace.ExpandedName(None, 'type')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 176, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
         ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
types._Automaton = _BuildAutomaton_3()




events._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'event'), eventpointer, scope=events, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 229, 3)))

def _BuildAutomaton_4 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_4
    del _BuildAutomaton_4
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(events._UseForTag(pyxb.namespace.ExpandedName(None, 'event')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 229, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
         ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
events._Automaton = _BuildAutomaton_4()




overviewdoc._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'section'), section, scope=overviewdoc, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 242, 3)))

def _BuildAutomaton_5 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_5
    del _BuildAutomaton_5
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(overviewdoc._UseForTag(pyxb.namespace.ExpandedName(None, 'section')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 242, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
         ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
overviewdoc._Automaton = _BuildAutomaton_5()




param._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'value'), value, scope=param, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 18, 3)))

param._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=param, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

def _BuildAutomaton_6 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_6
    del _BuildAutomaton_6
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 17, 3))
    counters.add(cc_0)
    cc_1 = fac.CounterCondition(min=0, max=None, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 18, 3))
    counters.add(cc_1)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(param._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 17, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_1, False))
    symbol = pyxb.binding.content.ElementUse(param._UseForTag(pyxb.namespace.ExpandedName(None, 'value')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 18, 3))
    st_1 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_1)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_0, False) ]))
    st_0._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_1, True) ]))
    st_1._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
param._Automaton = _BuildAutomaton_6()




event._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'params'), params, scope=event, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 28, 3)))

event._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=event, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

def _BuildAutomaton_7 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_7
    del _BuildAutomaton_7
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 27, 3))
    counters.add(cc_0)
    states = []
    final_update = None
    symbol = pyxb.binding.content.ElementUse(event._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 27, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(event._UseForTag(pyxb.namespace.ExpandedName(None, 'params')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 28, 3))
    st_1 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_1)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_0, False) ]))
    st_0._set_transitionSet(transitions)
    transitions = []
    st_1._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
event._Automaton = _BuildAutomaton_7()




enums._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'enum'), enum, scope=enums, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 38, 3)))

enums._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=enums, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

def _BuildAutomaton_8 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_8
    del _BuildAutomaton_8
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = None
    symbol = pyxb.binding.content.ElementUse(enums._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 37, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(enums._UseForTag(pyxb.namespace.ExpandedName(None, 'enum')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 38, 3))
    st_1 = fac.State(symbol, is_initial=False, final_update=final_update, is_unordered_catenation=False)
    states.append(st_1)
    transitions = []
    transitions.append(fac.Transition(st_1, [
         ]))
    st_0._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_1, [
         ]))
    st_1._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
enums._Automaton = _BuildAutomaton_8()




enum._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=enum, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

def _BuildAutomaton_9 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_9
    del _BuildAutomaton_9
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(enum._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 47, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
enum._Automaton = _BuildAutomaton_9()




value._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=value, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

def _BuildAutomaton_10 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_10
    del _BuildAutomaton_10
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 60, 3))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(value._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 60, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
value._Automaton = _BuildAutomaton_10()




defines._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=defines, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

defines._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'define'), define, scope=defines, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 69, 3)))

def _BuildAutomaton_11 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_11
    del _BuildAutomaton_11
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = None
    symbol = pyxb.binding.content.ElementUse(defines._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 68, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(defines._UseForTag(pyxb.namespace.ExpandedName(None, 'define')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 69, 3))
    st_1 = fac.State(symbol, is_initial=False, final_update=final_update, is_unordered_catenation=False)
    states.append(st_1)
    transitions = []
    transitions.append(fac.Transition(st_1, [
         ]))
    st_0._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_1, [
         ]))
    st_1._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
defines._Automaton = _BuildAutomaton_11()




define._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=define, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

def _BuildAutomaton_12 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_12
    del _BuildAutomaton_12
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(define._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 77, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
define._Automaton = _BuildAutomaton_12()




command._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=command, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

command._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'params'), params, scope=command, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 86, 3)))

command._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'returns'), returns, scope=command, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 92, 3)))

command._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'events'), events, scope=command, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 98, 3)))

def _BuildAutomaton_14 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_14
    del _BuildAutomaton_14
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 85, 3))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(command._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 85, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_15 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_15
    del _BuildAutomaton_15
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(command._UseForTag(pyxb.namespace.ExpandedName(None, 'params')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 86, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=st_0)

def _BuildAutomaton_16 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_16
    del _BuildAutomaton_16
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(command._UseForTag(pyxb.namespace.ExpandedName(None, 'returns')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 92, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=st_0)

def _BuildAutomaton_17 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_17
    del _BuildAutomaton_17
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 98, 3))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(command._UseForTag(pyxb.namespace.ExpandedName(None, 'events')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 98, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_13 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_13
    del _BuildAutomaton_13
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 85, 3))
    counters.add(cc_0)
    cc_1 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 98, 3))
    counters.add(cc_1)
    states = []
    sub_automata = []
    sub_automata.append(_BuildAutomaton_14())
    sub_automata.append(_BuildAutomaton_15())
    sub_automata.append(_BuildAutomaton_16())
    sub_automata.append(_BuildAutomaton_17())
    final_update = set()
    symbol = pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 84, 2)
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=True)
    st_0._set_subAutomata(*sub_automata)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
command._Automaton = _BuildAutomaton_13()




class_._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=class_, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

class_._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'command'), command, scope=class_, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 111, 4)))

class_._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'event'), event, scope=class_, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 112, 4)))

class_._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'enums'), enums, scope=class_, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 113, 4)))

class_._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'defines'), defines, scope=class_, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 119, 4)))

def _BuildAutomaton_18 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_18
    del _BuildAutomaton_18
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 109, 3))
    counters.add(cc_0)
    cc_1 = fac.CounterCondition(min=0, max=None, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 110, 3))
    counters.add(cc_1)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(class_._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 109, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_1, False))
    symbol = pyxb.binding.content.ElementUse(class_._UseForTag(pyxb.namespace.ExpandedName(None, 'command')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 111, 4))
    st_1 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_1)
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_1, False))
    symbol = pyxb.binding.content.ElementUse(class_._UseForTag(pyxb.namespace.ExpandedName(None, 'event')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 112, 4))
    st_2 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_2)
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_1, False))
    symbol = pyxb.binding.content.ElementUse(class_._UseForTag(pyxb.namespace.ExpandedName(None, 'enums')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 113, 4))
    st_3 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_3)
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_1, False))
    symbol = pyxb.binding.content.ElementUse(class_._UseForTag(pyxb.namespace.ExpandedName(None, 'defines')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 119, 4))
    st_4 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_4)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_0, False) ]))
    transitions.append(fac.Transition(st_2, [
        fac.UpdateInstruction(cc_0, False) ]))
    transitions.append(fac.Transition(st_3, [
        fac.UpdateInstruction(cc_0, False) ]))
    transitions.append(fac.Transition(st_4, [
        fac.UpdateInstruction(cc_0, False) ]))
    st_0._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_2, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_3, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_4, [
        fac.UpdateInstruction(cc_1, True) ]))
    st_1._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_2, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_3, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_4, [
        fac.UpdateInstruction(cc_1, True) ]))
    st_2._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_2, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_3, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_4, [
        fac.UpdateInstruction(cc_1, True) ]))
    st_3._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_2, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_3, [
        fac.UpdateInstruction(cc_1, True) ]))
    transitions.append(fac.Transition(st_4, [
        fac.UpdateInstruction(cc_1, True) ]))
    st_4._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
class_._Automaton = _BuildAutomaton_18()




type._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=type, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

def _BuildAutomaton_19 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_19
    del _BuildAutomaton_19
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(type._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 181, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
type._Automaton = _BuildAutomaton_19()




eventpointer._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'description'), pyxb.binding.datatypes.string, scope=eventpointer, location=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 65, 1)))

def _BuildAutomaton_20 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_20
    del _BuildAutomaton_20
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 221, 2))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(eventpointer._UseForTag(pyxb.namespace.ExpandedName(Namespace, 'description')), pyxb.utils.utility.Location('C:/SiliconLabs/ews/ews-ss/v5_1/st_wifi_ncp_brd4338a/scripts/bgapi_protocol/api/api.xsd', 222, 3))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
eventpointer._Automaton = _BuildAutomaton_20()

