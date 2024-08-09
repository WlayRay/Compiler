# c_compiler
项目只支持在Windows上的Visual Studio运行，支持VS2013 ~ 2022版本，适合作为学习编译原理的实践项目

 1. 输入一个含有原代码的txt文件，在输入一个空的txt文件，编译器进行词法分析并将结构输出在空的txt文件中
 
 2. 再将步骤1的源代码txt文件输入，编译器会进行语法和语义分析

 3. 最后再把步骤1输出的词法分析文件输入，编译器会解析成汇编语言并在虚拟机执行
  
PS（汇编代码不带表某个特定架构，而只是作为测试使用）

第一次输入的txt文件实例：

  ![image](https://github.com/WlayRay/c_compiler/assets/152136925/b03bbf26-c393-49a6-9a49-11391cea562e)

  

控制台的输入(输入的代码并不是上述图片内容)

<img width="695" alt="1710468089536" src="https://github.com/WlayRay/c_compiler/assets/152136925/c0f6f875-cefe-4ed9-8fca-2df017dc8893">

