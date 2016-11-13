<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcunoisedatainfo */

$this->title = 'Create Hcunoisedatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcunoisedatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcunoisedatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
