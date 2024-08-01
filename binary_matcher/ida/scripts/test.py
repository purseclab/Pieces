
import idc
import idaapi

idaapi.auto_wait()
idaapi.gen_flow_graph('sub_805C454', 'sub_805C454', None, 134595668, 134595684, idaapi.CHART_GEN_GDL)
idc.qexit(0)