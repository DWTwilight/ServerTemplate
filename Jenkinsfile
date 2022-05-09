pipeline {
    agent any
    triggers {
        githubPush()
    }
    stages {
        stage('Restore Submodules') {
            steps {
                sh 'git submodule update --init --recursive'
            }
        }
        stage('Config') {
            steps {
                sh 'cmake . -B./build'
            }
        }
        stage('Clean') {
            steps {
                sh 'cmake --build ./build --config Release --target clean'
            }
        }
        stage('Test') {
            steps {
                sh 'cd ./build/test && sudo cmake --build ../ && ctest'
            }
        }
        stage('Build') {
            steps {
                sh 'cmake --build ./build/ --config Release --target ws_server'
            }
        }
        stage('Deploy') {
            steps {
                sh '''for pid in $(lsof -t -i:5001); do
                       kill -9 $pid
               done'''
                sh 'export UV_THREADPOOL_SIZE=1024 && nohup ./build/bin/ws_server -j 2'
            }
        }
    }
}
