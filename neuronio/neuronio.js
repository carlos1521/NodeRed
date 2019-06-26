Perceptron = function (){

    var network = this;
    this.bias = 1;
    this.pesos = [];
    this.passo = 0.15;
    this.epocas = 1000;

    this.sigmoid = function(x){
        return (1 / (1 - Math.exp(-1 * x)));
    };

    this.init = function(passo, epocas){

        network.passo = passo;
        network.epocas = epocas;
        
    }

    this.initPesos = function(num){

        bias = parseInt(Math.random() * 10);

        for(var i = 0; i < num; i++){

            network.pesos[i] = parseInt(Math.random() * 10);
            
        }

    }

    this.train = function(data){
        network.initPesos(data[0].inputs.length);
        var epocas = 0;
        var error = true;

        while(error && epocas < network.epocas){
            var error = false;
            var diff = 0;

            for(var i = 0; i < data.length; i++){
                var result = network.run(data[i].inputs)
                if(result != data[i].output){
                    error = true;
                    diff = data[i].output - result;
                    network.recalcPesos(diff, data[i].inputs);
                } else {
                    error = false;
                }
            }

            console.log("Interação: " + epocas + " - Erro: " + diff.toFixed(12));
            epocas++;
        }
    }
    
    this.recalcPesos = function(val, inputs){

        for(var j = 0; j < network.pesos.length; j++){
            network.pesos[j] = network.pesos[j] + network.passo * val * inputs[j];
        }

    }

    this.run = function (inputs){

        var sum = 0;

        for(var j = 0; j < inputs.length; j++){
            sum += inputs[j] * network.pesos[j]
        }
        sum += network.bias;

        return network.sigmoid(sum);
    }
}