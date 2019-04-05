

/******************************��������*************************/
int binary_search(float search_table[],int length,float key);

/*
**************************************************************
*@ funciton			: int binary_search(int search_table[],int length,int key)  
*@ brief			: �۰���� 
*@ param 			: search_table�����ҵı�length��������ݵĳ��ȣ�key��Ҫ���ҵ�ֵ
*@ return			: ʧ�ܷ���-1���ɹ����ز��ҵ�ֵ���±�
***********************************************************************
*/
// �ǵݹ��۰�  
int binary_search(float search_table[],int length,float key) 
{  
    // ���λ������low�����λ������high���м�λ������mid  
  
    // �м�λ�õĿ������  
    // lengthΪ����ʱ��mid Ϊ���м�λ�� mid�������Ҳ�����ͬ����Ŀ��Ԫ��  
    // lengthΪż��ʱ��midΪ���м��������һ��Ԫ��   ���м�ΪС�������м��������һ��λ�ò���(Low+High)/2  
  
  
    // low��high�Ĺ�ϵ  
    // ���������low<high  
    // low==highʱ����ʣ�����һ����Ҫ�жϵ�Ԫ�أ���Ԫ�ؿ�����key��ͬ��Ҳ���ܲ�ͬ  
    // low>high δ�ҵ���key��ͬ��Ԫ��  
    int low=0;  
    int high=length-1;  
    int mid=(low+high)/2;  
  
    while(low<=high)
	{  
  
        mid=(low+high)/2;  
  
        //�ҵ���key��ȵ�һ��Ԫ��λ��  
        if((search_table[mid]==key)||((key>search_table[mid])&&(key<search_table[mid+1])))  
            return mid;
  
        if (search_table[mid]>key)  
            high=mid-1;  
        else  
            low=mid+1;  
    }  
  
    return -1;  
  
} 


 

