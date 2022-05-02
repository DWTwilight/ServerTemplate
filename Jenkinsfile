pipeline {
    agent any
    triggers {
        githubPush()
    }
    stages {
        stage('Restore Submodules'){
           steps{
               sh 'git submodule update --init --recursive'
            }
         }
        stage('Clean'){
           steps{
               sh 'cmake --build ./build --config Release --target clean'
            }
         }
        stage('Build'){
           steps{
               sh 'cmake --build ./build/ --config Release --target ws_server'
            }
         }
        stage('Deploy'){
             steps{
               sh '''for pid in $(lsof -t -i:5001); do
                       kill -9 $pid
               done'''
               sh 'export UV_THREADPOOL_SIZE=1024'
               sh './build/bin/ws_server -j 2'
             }
        }
    }
}