//
// Created by qzhang on 4/6/21.
//

#include <iostream>
#include <queue>
#define S 101

using namespace std;
struct N
{
  int s,n,m;
  int t;
};
int mark[S][S][S];
queue<N> Q;     // 队列
void AtoB(int &a,int sa,int &b,int sb){
  if(sb-b>=a){
    b+=a;
    a=0;
  }
  else{
    a-=sb-b;
    b=sb;
  }
}
int BFS(int s, int n, int m ){
  while(!Q.empty()){
    N now =Q.front();
    Q.pop();

    int a,b,c;

    a=now.s;
    b=now.n;
    c=now.m;
    AtoB(a,s,b,n);
    if(mark[a][b][c]==false){
      mark[a][b][c]=true;
      N tmp;
      tmp.s=a;
      tmp.n=b;
      tmp.m=c;
      tmp.t=now.t+1;
      if(a==s/2 && b==s/2){
        return tmp.t;
      }
      if(a==s/2 && c==s/2){
        return tmp.t;
      }
      if(b==s/2 && c==s/2){
        return tmp.t;
      }
      Q.push(tmp);
    }

    a=now.s;
    b=now.n;
    c=now.m;
    AtoB(b,n,a,s);
    if(mark[a][b][c]==false){
      mark[a][b][c]=true;
      N tmp;
      tmp.s=a;
      tmp.n=b;
      tmp.m=c;
      tmp.t=now.t+1;
      if(a==s/2 && b==s/2){
        return tmp.t;
      }
      if(a==s/2 && c==s/2){
        return tmp.t;
      }
      if(b==s/2 && c==s/2){
        return tmp.t;
      }
      Q.push(tmp);
    }

    a=now.s;
    b=now.n;
    c=now.m;
    AtoB(a,s,c,m);
    if(mark[a][b][c]==false){
      mark[a][b][c]=true;
      N tmp;
      tmp.s=a;
      tmp.n=b;
      tmp.m=c;
      tmp.t=now.t+1;
      if(a==s/2 && b==s/2){
        return tmp.t;
      }
      if(a==s/2 && c==s/2){
        return tmp.t;
      }
      if(b==s/2 && c==s/2){
        return tmp.t;
      }
      Q.push(tmp);
    }

    a=now.s;
    b=now.n;
    c=now.m;
    AtoB(c,m,a,s);
    if(mark[a][b][c]==false){
      mark[a][b][c]=true;
      N tmp;
      tmp.s=a;
      tmp.n=b;
      tmp.m=c;
      tmp.t=now.t+1;
      if(a==s/2 && b==s/2){
        return tmp.t;
      }
      if(a==s/2 && c==s/2){
        return tmp.t;
      }
      if(b==s/2 && c==s/2){
        return tmp.t;
      }
      Q.push(tmp);
    }

    a=now.s;
    b=now.n;
    c=now.m;
    AtoB(b,n,c,m);
    if(mark[a][b][c]==false){
      mark[a][b][c]=true;
      N tmp;
      tmp.s=a;
      tmp.n=b;
      tmp.m=c;
      tmp.t=now.t+1;
      if(a==s/2 && b==s/2){
        return tmp.t;
      }
      if(a==s/2 && c==s/2){
        return tmp.t;
      }
      if(b==s/2 && c==s/2){
        return tmp.t;
      }
      Q.push(tmp);
    }

    a=now.s;
    b=now.n;
    c=now.m;
    AtoB(c,m,b,n);
    if(mark[a][b][c]==false){
      mark[a][b][c]=true;
      N tmp;
      tmp.s=a;
      tmp.n=b;
      tmp.m=c;
      tmp.t=now.t+1;
      if(a==s/2 && b==s/2){
        return tmp.t;
      }
      if(a==s/2 && c==s/2){
        return tmp.t;
      }
      if(b==s/2 && c==s/2){
        return tmp.t;
      }
      Q.push(tmp);
    }
  }
  return -1;
}
int main(int argc, const char * argv[]) {
  int s,n,m;
  while(cin>>s>>n>>m){
    if(s==0&&n==0&&m==0){
      break;
    }
    if (s%2==1) {
      cout<<"NO"<<endl;
      continue;
    }
    for(int i=0;i<s;i++){
      for(int j = 0; j < n; j++)
      {
        for(int k = 0; k < m; k++)
        {
          mark[i][j][k]=false;
        }
      }
    }

    while(!Q.empty()){
      Q.pop();
    }

    N begin;
    begin.s=s;
    begin.n=begin.m=begin.t=0;
    mark[s][0][0]=true;
    Q.push(begin);

    int res=BFS(s,n,m);
    if(res==-1){
      cout<<"NO"<<endl;;
    }
    else{
      cout<<res<<endl;
    }
  }
  return 0;
}
