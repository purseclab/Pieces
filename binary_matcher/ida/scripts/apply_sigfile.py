
import idc
import idaapi

if __name__ == "__main__":
    idc.auto_wait()
    if idaapi.plan_to_apply_idasgn("/home/sloan/testing/dispatch/ida/scripts/math.sig") == 0:
        idaapi.qexit(1)
    idaapi.qexit(0)