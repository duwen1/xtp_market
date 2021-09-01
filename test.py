from vnpy.event import EventEngine
from vnpy.trader.engine import MainEngine
from vnpy.trader.ui import MainWindow, create_qapp

from vnpy_xtp import XtpGateway
from datetime import datetime, time
from vnpy.gateway.ctp import CtpGateway
from vnpy.app.cta_strategy import CtaStrategyApp
from time import sleep
import sys

ctp_setting = {
    "用户名": "071966",
    "密码": "Lenovo20153",
    "经纪商代码": "9999",
    "交易服务器": "180.168.146.187",
    "行情服务器": "180.168.146.187",
    "产品名称": "",
    "授权编码": "0000000000000000",
    "产品信息": "simnow_client_test.0.0"
}

my_settings= {
    "账号": "215007711",
    "密码": "wYX72mrG",
    "客户号": 1,
    "行情地址": "119.3.103.38",
    "行情端口": 6002,
    "交易地址": "122.112.139.0",
    "交易端口": 6102,
    "行情协议": "TCP",
    "日志级别":  "DEBUG",
    "授权码": "b8aa7173bba3470e390d787219b2112e"
}
DAY_START = time(8, 45)
DAY_END = time(15, 0)

NIGHT_START = time(20, 45)
NIGHT_END = time(2, 45)

def check_trading_period():
    """"""
    current_time = datetime.now().time()

    trading = False
    if (
        (current_time >= DAY_START and current_time <= DAY_END)
        or (current_time >= NIGHT_START)
        or (current_time <= NIGHT_END)
    ):
        trading = True

    return trading


def main():
    """主入口函数"""
    # qapp = create_qapp()
    event_engine = EventEngine()
    main_engine = MainEngine(event_engine)
    main_engine.add_gateway(XtpGateway)



    main_engine.connect(my_settings, "XTP")
    main_engine.write_log("连接XTP接口")


    # event_engine = EventEngine()
    # main_engine = MainEngine(event_engine)
    # main_engine.add_gateway(CtpGateway)
    # cta_engine = main_engine.add_app(CtaStrategyApp)
    # main_engine.write_log("主引擎创建成功")



    # main_engine.connect(ctp_setting, "CTP")
    # main_engine.write_log("连接CTP接口")

    sleep(10)
    while True:
        sleep(10)
    # cta_engine.init_engine()
    # main_engine.write_log("CTA策略初始化完成")

    # cta_engine.init_all_strategies()
    # sleep(60)   # Leave enough time to complete strategy initialization
    # main_engine.write_log("CTA策略全部初始化")

    # cta_engine.start_all_strategies()
    # main_engine.write_log("CTA策略全部启动")

    # while True:
    #     sleep(10)

    #     trading = check_trading_period()
    #     if not trading:
    #         print("关闭子进程")
    #         main_engine.close()
    #         sys.exit(0)



    # xtp_gw = XtpGateway(main_engine)
    # xtp_gw.connect(setting=my_settings)
    # xtp_gw.subscribe()
    # event_engine = EventEngine()
    # main_engine = MainEngine(event_engine)
    # main_engine.add_gateway(XtpGateway)
    # main_window = MainWindow(main_engine, event_engine)
    # main_window.showMaximized()
    # qapp.exec()


if __name__ == "__main__":
    main()
