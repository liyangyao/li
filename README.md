###li
>"li" is a library use for Qt5, author liyangyao@gmail.com
>need c++ 11

include

* BlockingQueue
* Singleton
* Debug
* WriteDump
* WmiQuery

		void getCPUUsage()
		{
		    m_query.exec("select LoadPercentage from Win32_Processor");
		    if(m_query.next())
		    {
		        qDebug()<<"Cpu LoadPercentage:"<< m_query.value(0).toString();
		    }
		    else{
		        qDebug()<<m_query.lastError();
		    }
		}

